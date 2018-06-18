#ifndef __ROVICOMPONENT_H__
#define __ROVICOMPONENT_H__

#include <string>

class RoviComponent {
public:
    virtual void redirectedMqttMessage(const std::string& topic, const std::string& payload) = 0;
// private:
    std::string name;    
};

#endif /* __ROVICOMPONENT_H__ */