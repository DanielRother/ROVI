#include "BaseClasses/RoviDevice.hpp"
#include "LED/ColorTypes.h"

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>
#include <FileIOUtils.hpp>

RoviDevice::RoviDevice(Basecamp& iot)
    : iot(iot), name(""), room(""), baseTopic(""), statusTopic("")
{}

void RoviDevice::setupRovi() {
    Serial << "--- RoviDevice::setupRovi() ---" << endl;
    // Needs to be in a dedicated method, 'cause iot.begin() in the ctor crashes the application...
    // iot.begin();

    name = iot.configuration.get("DeviceName").c_str();
    room = addVariableToIotConfig("room", "/haus/etage/zimmer");
    iot.configuration.save();

    baseTopic   = fullfile(room, name); 
    statusTopic = fullfile(baseTopic, "status");

    Serial << "  baseTopic: " << baseTopic << endl;
    Serial << "  statusTopic: " << statusTopic << endl;
}

  void RoviDevice::addComponent(std::shared_ptr<RoviComponent> component) {
      this->components[component->name] = component;
  }


//****************************************//
// MQTT interface methods
//****************************************//
void RoviDevice::mqttConnected(bool sessionPresent) {
    Serial << "MQTT verbunden!" << endl;
    
    iot.mqtt.subscribe(fullfile(baseTopic, "#").c_str(), to_underlying(MQTTQoSClasses::EXACTLY_ONE));

    bool retainFlag = true;
    iot.mqtt.publish(statusTopic.c_str(), to_underlying(MQTTQoSClasses::EXACTLY_ONE), retainFlag, "online");
    // TODO: Abmelden? Ungetestet
    iot.mqtt.setWill(statusTopic.c_str(), to_underlying(MQTTQoSClasses::EXACTLY_ONE), retainFlag, "offline");
    // iot.mqtt.setKeepAlive
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

    // Get responsible component and redirect shortened message, if component exists
    auto responsibleCompontent = components.find(getResponsibleComponent(topicString));
    if(responsibleCompontent != components.end()) {
        std::string topicWithoutBasestring = removeBaseTopic(topicString);
        responsibleCompontent->second->redirectedMqttMessage(topicWithoutBasestring, payload);
    }

    // TODO: Redirect message to components
    std::string lightTopic = std::string(baseTopic.c_str()) + "light/";
    if(topicString == lightTopic) {
        Serial << "lightTopic received\n";

        RGBAColor rgba(payloadString);

        Serial << "Color: " << rgba.toString().c_str() << endl;
    }
}

std::string RoviDevice::addVariableToIotConfig(const std::string& name, const std::string& defaultValue) {
    // Convertion required due to interface...
    String nameConv         = String(name.c_str());
    String defaultValueConv = String(defaultValue.c_str());

    String var = iot.configuration.get(nameConv);

    if(var.length() == 0) {
        Serial << "  Variable '" << nameConv << "' is empty. Set to default (" << defaultValueConv << ")\n";
        iot.configuration.set(nameConv, defaultValueConv);
    } else {
        Serial << "  Variable '" << nameConv << "': " << var << endl;
    }

    iot.web.addInterfaceElement(
        "html_" + nameConv,
        "input",
        "Variable " + nameConv + ":",
        "#configform",
        nameConv
    );

    return std::string(iot.configuration.get(nameConv).c_str());
}

std::string RoviDevice::removeBaseTopic(const std::string& mqttTopic) {
    if(mqttTopic.find(baseTopic) == std::string::npos) {
        return mqttTopic;
    }
    std::string topicWithoutBasestring = mqttTopic.substr(baseTopic.size() + 1, mqttTopic.size());

    return topicWithoutBasestring;
}

std::string RoviDevice::getResponsibleComponent(const std::string& mqttTopic) {
    std::string topicWithoutBasestring = removeBaseTopic(mqttTopic);
    std::string compontent             = topicWithoutBasestring.substr(0, topicWithoutBasestring.find("/"));

    // Serial << "Topic:       "   << mqttTopic << endl;
    // Serial << "Basetopic:   "   << baseTopic << endl;
    // Serial << "withoutBase: "   << topicWithoutBasestring << endl;
    // Serial << "compontent:  "   << compontent << endl;

    return compontent;  
}
