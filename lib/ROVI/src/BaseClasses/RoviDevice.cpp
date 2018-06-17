#include "BaseClasses/RoviDevice.hpp"
#include "LED/ColorTypes.h"

#include <prettyprint.hpp>
#include <arduinoIostream.hpp>

RoviDevice::RoviDevice(Basecamp& iot)
    : iot(iot), name(""), room(""), baseTopic(""), statusTopic("")
{}

void RoviDevice::setupRovi() {
    Serial << "--- RoviDevice::setupRovi() ---" << endl;
    // Needs to be in a dedicated method, 'cause iot.begin() in the ctor crashes the application...
    // iot.begin();

    name = iot.configuration.get("DeviceName");
    room = addVariableToIotConfig("room", "/haus/etage/zimmer");
    iot.configuration.save();

    // TODO: fullfile()
    baseTopic = room + "/" + name + "/"; 
    statusTopic = baseTopic + "status/";
}

//****************************************//
// MQTT interface methods
//****************************************//
void RoviDevice::mqttConnected(bool sessionPresent) {
    Serial << "MQTT verbunden!" << endl;

    // Noch aus dem Beispiel
//   subTopic = iot.hostname + "/sendtemp";
//   pubTopic = iot.hostname + "/status";
//   tempTopic = iot.hostname + "/temp";
//   iot.mqtt.subscribe(subTopic.c_str(), 2);
//   iot.mqtt.publish(subTopic.c_str(), 1, true, "online");        // <- Publishes a test message

//   lightTopic = "/house/lightabc/set";
    iot.mqtt.subscribe((baseTopic + "#").c_str(), 2);

    int qosLevel = 1;
    bool retainFlag = true;
    iot.mqtt.publish(statusTopic.c_str(), qosLevel, retainFlag, "online");    // TODO: Explain magic numbers
}

void RoviDevice::mqttSubscribed(uint16_t packetId, uint8_t qos) {
  Serial << "MQTT Abonnement erfolgreich" << endl;
}

void RoviDevice::mqttPublished(uint16_t packetId) {
  Serial << "MQTT-Nachricht veröffentlicht" << endl;
}

void RoviDevice::mqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    Serial << "Neue MQTT-Nachricht:\n" << "Topic: " << topic << "\nPayload: " << payload << endl;

    std::string topicString(topic);
    std::string payloadString(payload);

    std::string lightTopic = std::string(baseTopic.c_str()) + "light/";
    if(topicString == lightTopic) {
        Serial << "lightTopic received\n";

        RGBAColor rgba(payloadString);

        Serial << "Color: " << rgba.toString().c_str() << endl;
    }
}

String RoviDevice::addVariableToIotConfig(String name, String defaultValue) {
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