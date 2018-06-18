#ifndef __ROVICOMPONENT_H__
#define __ROVICOMPONENT_H__

#include <string>

class RoviComponent {
public:
    RoviComponent(const std::string& name) 
        : name(name)
        {};

    RoviComponent(const RoviComponent& other) = default;
    virtual ~RoviComponent() = default;

    virtual void redirectedMqttMessage(const std::string& topic, const std::string& payload) = 0;
// protected:
    std::string name;    
};

#endif /* __ROVICOMPONENT_H__ */