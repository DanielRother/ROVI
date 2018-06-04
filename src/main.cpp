#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <arduinoIostream.hpp>

#include <LED/ColorTypes.h>

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

// 'global' variables
String room;
String temp;
int curTemp = 1;

String subTopic;
String pubTopic;
String tempTopic;

std::string lightTopic;

String getTemp() {

  String temp = String(25 + curTemp++);
  curTemp %= 10;

  iot.configuration.set("my-Temperature", temp);
  iot.configuration.save();
  iot.mqtt.publish("stat/frontdoor/battery", 1, true, "empty" );

  return temp;
}

//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
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


//***************************************************************************//
// Implementation of MQTT methods
//***************************************************************************//
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
  Serial << "Abonnement erfolgreich\n";
};

void mqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial << "Neue MQTT-Nachricht:\n" << "Topic: " << topic << "\nPayload: " << payload << "\n";

  std::string topicString(topic);
  std::string payloadString(payload);

  iot.mqtt.publish(tempTopic.c_str(), 1, true, getTemp().c_str());

  if(topicString == lightTopic) {
    Serial << "lightTopic received\n";

    // TODO: Move to ctor of colorTuple
    //       and cleanup
    // Palyoad format: RGBA(54,255,46, 255)
    // 1. First extract the value string, i.e. the numbers
    auto valStart = payloadString.find('(') + 1; 
    auto valEnd   = payloadString.find(')'); 
    std::string values = payloadString.substr(valStart, valEnd - valStart);

    Serial << "start: " << valStart << "\tend: " << valEnd << "\n" << "Values: " << values.c_str() << "\n";

    // 2. Extract the actual RGB values
    std::stringstream ss(values);
    std::string token;
    std::vector<int> cont;
    while (std::getline(ss, token, ',')) {
        int curVal = atoi(token.c_str());
        cont.push_back(curVal);

        Serial << "curVal: " << curVal << "\n";
    }

    std::stringstream ss2;
    ss2 << cont << std::endl;
    Serial << "cont new: " << ss2.str().c_str() << "\n";

    colorTuple ct;
    if(cont.size() == 4) {
      ct = colorTuple(cont[0], cont[1], cont[2], cont[3]);
    } else if(cont.size() == 3) {
      ct = colorTuple(cont[0], cont[1], cont[2]);  
    }

    Serial << "ctString: " << ct.toString().c_str() << "\nmanual B: " <<  (int) ct.B << "\n";
  }
};

void mqttPublished(uint16_t packetId) {
  Serial << "MQTT-Nachricht veröffentlicht\n";
};

//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
  delay(10000);
  temp = getTemp();
  Serial << "Temperatur: " << temp << "\n";
  iot.mqtt.publish(tempTopic.c_str(), 1, true, temp.c_str());

  Serial << "Serial streaming foobar\n";
}