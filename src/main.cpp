#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>

#include <Basecamp.hpp>
// Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
Basecamp iot{
  Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
};

// Forward declaration of MQTT call-backs
void mqttConnected(bool sessionPresent);
void mqttSubscribed(uint16_t packetId, uint8_t qos);
void mqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void mqttPublished(uint16_t packetId);

//#include <MyUtils.h>
//#include <prettyprint.hpp>

//// Dieses Beispiel verwendet einen DHT11 als Temperatursensor
//// Sie benötigen https://github.com/beegee-tokyo/DHTesp als Bibliothek
//#include <DHTesp.h>
//DHTesp dht;
//const int dhtPin = 17;
String room;
String temp;
int curTemp = 1;

String subTopic;
String pubTopic;
String tempTopic;

std::string lightTopic;

struct colorTuple {
  colorTuple(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
    : R(R), G(G), B(B), A(A)
  {}

  colorTuple(uint8_t R, uint8_t G, uint8_t B)
    : R(R), G(G), B(B), A(255)
  {}

  colorTuple()
      : R(255), G(255), B(255), A(255)
  {}

  
  uint8_t R;
  uint8_t G;
  uint8_t B;
  uint8_t A;

  std::string toString() {
    std::stringstream ret;
    ret << "R: " << (uint32_t) R << "; G: " << (uint32_t) G << "; B: " << (uint32_t) B << "; A: " << (uint32_t) A << ";" << std::endl;
    
    ret << "RGBA(" << (uint32_t) R << "," << (uint32_t) G << "," << (uint32_t) B << "," << (uint32_t) A << ")" << std::endl;
    return ret.str();
  }
};

String getTemp() {

  String temp = String(25 + curTemp++);
  curTemp %= 10;

//  if (dht.getStatus() != 0) {
//    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
//    temp = "-274";
//  }
  iot.configuration.set("my-Temperature", temp);
  iot.configuration.save();
  iot.mqtt.publish("stat/frontdoor/battery", 1, true, "empty" );

  return temp;
}

void setup() {
  //dht.setup(dhtPin, DHTesp::DHT11);
  iot.begin();
  iot.mqtt.onConnect(mqttConnected);
  iot.mqtt.onSubscribe(mqttSubscribed);
  iot.mqtt.onMessage(mqttMessage);

  temp = getTemp();
  iot.configuration.set("my-Temperature", temp);
  iot.configuration.save();

  room = iot.configuration.get("my-Room");


  iot.web.addInterfaceElement(
    "tempDisplay",
    "input",
    "Aktuelle Temperatur:",
    "#configform",
    "my-Temperature"
  );
  iot.web.setInterfaceElementAttribute(
    "tempDisplay",
    "readonly",
    "true"
  );
  iot.mqtt.onPublish(mqttPublished);

  iot.web.server.on(
    "/temperature",
    HTTP_GET,
    [](AsyncWebServerRequest * request)
  {
    AsyncWebServerResponse *response =
      request->beginResponse(
        200,
        "text/plain",
        getTemp()
      );
    request->send(response);
  });

  Serial.println("PlatformIO setup");
}



void mqttConnected(bool sessionPresent) {
  Serial.println("MQTT verbunden!");
  subTopic = iot.hostname + "/sendtemp";
  pubTopic = iot.hostname + "/status";
  tempTopic = iot.hostname + "/temp";
  iot.mqtt.subscribe(subTopic.c_str(), 2);
  iot.mqtt.publish(subTopic.c_str(), 1, true, "online");

  lightTopic = "/house/lightabc/set";
  iot.mqtt.subscribe(lightTopic.c_str(), 2);
};

void mqttSubscribed(uint16_t packetId, uint8_t qos) {
  Serial.println("Abonnement erfolgreich");
};

void mqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Neue MQTT-Nachricht:");
  Serial.print("Topic:");
  Serial.println(topic);
  Serial.print("Payload:");
  Serial.println(payload);

  std::string topicString(topic);
  std::string payloadString(payload);

  iot.mqtt.publish(tempTopic.c_str(), 1, true, getTemp().c_str());

  if(topicString == lightTopic) {
    Serial.println("lightTopic received");

    // TODO: Move to ctor of colorTuple
    //       and cleanup
    // Palyoad format: RGBA(54,255,46, 255)
    // 1. First extract the value string, i.e. the numbers
    auto valStart = payloadString.find('(') + 1; 
    auto valEnd   = payloadString.find(')'); 
    std::string values = payloadString.substr(valStart, valEnd - valStart);

    std::stringstream msg;
    msg << "start: " << valStart << "\tend: " << valEnd << std::endl;
    Serial.println(msg.str().c_str());
    Serial.print("Values: ");
    Serial.println(values.c_str());

    // 2. Extract the actual RGB values
    std::stringstream ss(values);
    std::string token;
    std::vector<int> cont;
    while (std::getline(ss, token, ',')) {
        int curVal = atoi(token.c_str());
        cont.push_back(curVal);

        Serial.print("curVal: ");
        Serial.println(curVal);
    }

    std::stringstream ss2;
    ss2 << cont << std::endl;
    Serial.print("cont: ");
    Serial.println(ss2.str().c_str());

    colorTuple ct;
    if(cont.size() == 4) {
      ct = colorTuple(cont[0], cont[1], cont[2], cont[3]);
    } else if(cont.size() == 3) {
      ct = colorTuple(cont[0], cont[1], cont[2]);  
    }

    auto ctString = ct.toString();
    Serial.print("ctString: ");
    Serial.println(ctString.c_str());
    Serial.print("manual B:");
    Serial.println(ct.B);

  }
};

void mqttPublished(uint16_t packetId) {
  Serial.println("MQTT-Nachricht veröffentlicht");
};


void loop() {
  delay(10000);
  Serial.print("Temperatur: ");
  temp = getTemp();
  Serial.println(temp);
  iot.mqtt.publish(tempTopic.c_str(), 1, true, temp.c_str());
}