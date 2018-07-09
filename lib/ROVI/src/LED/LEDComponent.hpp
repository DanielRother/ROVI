#ifndef __LEDCOMPONENT_H__
#define __LEDCOMPONENT_H__

#include <Arduino.h>

#include <string>
#include <algorithm>
#include <thread>

#include <ArduinoIostream.hpp>

#include "BaseClasses/RoviComponent.hpp"
#include "LED/ColorTypes.h"

#include <ThreadUtil.hpp>


class LEDComponent : public RoviComponent {
public:
    LEDComponent(const std::string& name = "LED");
    LEDComponent(const LEDComponent& other) = default;
    virtual ~LEDComponent() = default;

    // The convertion between string and concrete data type can be done in this abstract class
    // But the actual action have to be implemented in an derived class
    // Names due to overloaded function and bind make problems and would led to long typecasts...
    virtual void setPowerMQTT(const std::string& payload);
    virtual void setColorMQTT(const std::string& payload);
    virtual void setBrightnessMQTT(const std::string& payload);
    virtual void setEffectMQTT(const std::string& payload);

    void colorFlow();


    // Interfac for the derived class
    virtual void setColor(const std::shared_ptr<Color>& color) = 0;
    virtual void setPower(bool power) = 0;
    virtual void setBrightness(uint8_t brightness) = 0;
    // virtual void setEffect(something) = 0;

// protected:
    std::shared_ptr<Color> m_rgb;

    std::thread t;
};


#endif /* __LEDCOMPONENT_H__ */