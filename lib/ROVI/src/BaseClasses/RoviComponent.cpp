#include "RoviComponent.hpp"
#include "RoviDevice.hpp"

#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm>

#include <FileIOUtils.hpp>
#include <ArduinoIostream.hpp>
#include <EnumUtils.hpp>

namespace Rovi {
    RoviComponent::RoviComponent(const std::string& name) 
        : name(toLower(name))
        {};

    void RoviComponent::redirectedMqttMessage(const std::string& topic, const std::string& payload) {
        Serial << "--- RoviComponent::redirectedMqttMessage() ---" << endl;

        // Get the action out of the topic string
        std::vector<std::string> mqttParts = splitString(topic, '/');

        if(mqttParts.size() < 2) {
            Serial << "  Something strange happened when splitting the MQTT topic... Returning" << endl;
            return;
        }

        // TBD: What happens to the possible other parts?
        auto h = handler.find(mqttParts[1]);

        if(h == handler.end()) {
            Serial << "  No such component action (" << name << "::" << mqttParts[1] << "). Returning" << endl;
            delay(50);
            return;
        }

        // Call the registered callback function
        h->second(payload);
    }

    // void RoviComponent::setRoviDevice(std::shared_ptr<RoviDevice> newDevice) {
    void RoviComponent::setRoviDevice(RoviDevice* newDevice) {
        Serial << "Set device " << newDevice->name << " for component " << name << endl; 
        device = newDevice;
    }

    std::string RoviComponent::getMqttBaseTopic() const {
        if(device == nullptr) {
            Serial << "Warning: No rovi device connected to component " << name << endl;
            return name;
        }

        return device->getBaseTopic() + "/" + name;
    }
    void RoviComponent::publishMQTTMessage(std::string& subtopic, std::string& payload) const {
        auto baseTopic = getMqttBaseTopic();
        std::string topic = baseTopic + "/" + subtopic;

        Serial << "Publish message \"" << payload << "\" to topic \"" << topic << "\"" << endl;  
        std::transform(topic.begin(),   topic.end(),   topic.begin(),   ::tolower);
        std::transform(payload.begin(), payload.end(), payload.begin(), ::tolower);

        if(device != nullptr) {
            Serial << "Try to send" << endl;
            auto packetID = device->iot.mqtt.publish(topic.c_str(), to_underlying(RoviDevice::MQTTQoSClasses::AT_MOST_ONE), false, payload.c_str());
            Serial << "PacketID: " << packetID << endl;
        } else {
            Serial << "Publishing of MQTT message not possible, because there is no device connected..." << endl;
        }
    }
}