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


#include "BaseClasses/RoviDevice.hpp"
RoviDevice myRovi(iot);

//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  Serial << "--- setup() ---" << endl;

  sleep(5);
  iot.begin();
  myRovi.setupRovi();

  // TODO: Is there an easier way to connect to methods???
  iot.mqtt.onConnect([&](bool sessionPresent) {myRovi.mqttConnected(sessionPresent);});
  iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {myRovi.mqttSubscribed(packetId, qos);});
  iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
    size_t len, size_t index, size_t total) {myRovi.mqttMessage(topic, payload, properties, len, index, total);});
  iot.mqtt.onPublish([&](uint16_t packetId) {myRovi.mqttPublished(packetId);});


  // Noch aus dem Beispiel zum hinzufügen zum Webinterface
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
}


//***************************************************************************//
// Implementation of MQTT methods
//***************************************************************************//
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