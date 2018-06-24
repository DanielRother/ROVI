#ifndef __LEDCOMPONENT_H__
#define __LEDCOMPONENT_H__

#include <Arduino.h>

#include <string>

#include <ArduinoIostream.hpp>

#include "BaseClasses/RoviComponent.hpp"
#include "LED/ColorTypes.h"

class LEDComponent : public RoviComponent {
public:
    LEDComponent(const std::string& name = "LED") 
        : RoviComponent(name)
        {};

    LEDComponent(const LEDComponent& other) = default;
    virtual ~LEDComponent() = default;

    virtual void redirectedMqttMessage(const std::string& topic, const std::string& payload) override {
        Serial << "LEDComponent::redirectedMqttMessage()" << endl;

        // TODO: Check topic and payload
        rgba = RGBAColor(payload);

        Serial << "Color: " << rgba.toString().c_str() << endl;

        updateColor(rgba);
    }

    virtual void updateColor(const RGBAColor& rgba) = 0;
// protected:
    RGBAColor rgba;
};

#endif /* __LEDCOMPONENT_H__ */