#ifndef __ROVICOMPONENT_H__
#define __ROVICOMPONENT_H__

#include <Arduino.h>

#include <string>
// #include <vector>
// #include <iostream>
// #include <iterator>
#include <functional>
#include <map>
#include <memory>


class RoviDevice;

// Helper for splitting the string	
template<char delimiter>
class WordDelimitedBy : public std::string
{};

class RoviComponent : public std::enable_shared_from_this<RoviComponent>  {
public:
    RoviComponent(const std::string& name);
    RoviComponent(const RoviComponent& other) = default;
    virtual ~RoviComponent() = default;

    virtual void redirectedMqttMessage(const std::string& topic, const std::string& payload);

    // TODO: Fix me! shared_from_this() in RoviDevice crashes application... :( Therefore, raw pointer is used
    // void setRoviDevice(std::shared_ptr<RoviDevice> newDevice);
    void setRoviDevice(RoviDevice* newDevice);

    std::string getMqttBaseTopic() const;
    void publishMQTTMessage(std::string& subtopic, std::string& payload) const;

// protected:

    std::string name;    
    std::map<std::string, std::function<void(const std::string&)>> handler;
    // std::shared_ptr<RoviDevice> device;
    RoviDevice* device;
};

#endif /* __ROVICOMPONENT_H__ */