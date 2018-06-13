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


class RoviDevice {
public:
  RoviDevice(Basecamp& iot)
    : iot(iot), name(""), room("")
  {}

  void setupRovi() {
    Serial << "--- RoviDevice::setupRovi() ---" << endl;
    // Needs to be in a dedicated method, 'cause iot.begin() in the ctor crashes the application...
    // iot.begin();

    name = iot.configuration.get("DeviceName");
    room = addVariableToIotConfig("room", "/haus/etage/zimmer");
    iot.configuration.save();
  }


// private:
  String addVariableToIotConfig(String name, String defaultValue) {
    String var = iot.configuration.get(name);
    
    if(var.length() == 0) {
      Serial << "  Variable '" << name << "' is empty. Set to default (" << defaultValue << ")\n";
      iot.configuration.set(name, defaultValue);
    } else {
      Serial << "  Variable '" << name << "': " << var << endl;
    }

    iot.web.addInterfaceElement(
      "html_" + name,
      "input",
      "Variable " + name + ":",
      "#configform",
      name
    );

    return iot.configuration.get(name);
  }

  Basecamp& iot;
  String name;
  String room;
};


RoviDevice myRovi(iot);

//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  sleep(5);
  iot.begin();
  myRovi.setupRovi();

  // TODO: Move to RoviBaseclass
  iot.mqtt.onConnect(mqttConnected);
  iot.mqtt.onSubscribe(mqttSubscribed);
  iot.mqtt.onMessage(mqttMessage);
  iot.mqtt.onPublish(mqttPublished);



  // Noch aus dem Beispiel zum hinzufügen zum Webinterface
  // temp = getTemp();
  // iot.configuration.set("my-Temperature", temp);
  // iot.configuration.save();

  // iot.web.addInterfaceElement(
  //   "tempDisplay",
  //   "input",
  //   "Aktuelle Temperatur:",
  //   "#configform",
  //   "my-Temperature"
  // );
  // iot.web.setInterfaceElementAttribute(      // <- Das setzt das Element noch auf Readonly
  //   "tempDisplay",
  //   "readonly",
  //   "true"
  // );

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
  Serial << "Neue MQTT-Nachricht:\n" << "Topic: " << topic << "\nPayload: " << payload << endl;

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

    Serial << "start: " << valStart << "\tend: " << valEnd << endl << "Values: " << values.c_str() << endl;

    // 2. Extract the actual RGB values
    std::stringstream ss(values);
    std::string token;
    std::vector<int> cont;
    while (std::getline(ss, token, ',')) {
        int curVal = atoi(token.c_str());
        cont.push_back(curVal);

        Serial << "curVal: " << curVal << endl;
    }

    std::stringstream ss2;
    ss2 << cont << std::endl;
    Serial << "cont new: " << ss2.str().c_str() << endl;

    colorTuple ct;
    if(cont.size() == 4) {
      ct = colorTuple(cont[0], cont[1], cont[2], cont[3]);
    } else if(cont.size() == 3) {
      ct = colorTuple(cont[0], cont[1], cont[2]);  
    }

    Serial << "ctString: " << ct.toString().c_str() << "\nmanual B: " <<  (int) ct.B << endl;
  }
};

void mqttPublished(uint16_t packetId) {
  Serial << "MQTT-Nachricht veröffentlicht\n";
};

//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
  sleep(5);
  temp = getTemp();
  Serial << "Temperatur: " << temp << endl;
  iot.mqtt.publish(tempTopic.c_str(), 1, true, temp.c_str());


  Serial << "myRovi " << myRovi.name  << " is in room " << myRovi.room << endl;
}