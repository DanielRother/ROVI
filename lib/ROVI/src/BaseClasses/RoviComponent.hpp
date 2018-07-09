#ifndef __ROVICOMPONENT_H__
#define __ROVICOMPONENT_H__

#include <Arduino.h>

#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <functional>
#include <map>

#include <FileIOUtils.hpp>

// Helper for splitting the string	
template<char delimiter>
class WordDelimitedBy : public std::string
{};

class RoviComponent {
public:
    RoviComponent(const std::string& name) 
        : name(name)
        {};

    RoviComponent(const RoviComponent& other) = default;
    virtual ~RoviComponent() = default;

    virtual void redirectedMqttMessage(const std::string& topic, const std::string& payload) {
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
            return;
        }

        // Call the registered callback function
        h->second(payload);
    }    
// protected:

    std::string name;    
    std::map<std::string, std::function<void(const std::string&)>> handler;
};

#endif /* __ROVICOMPONENT_H__ */