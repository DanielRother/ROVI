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
    LEDComponent(const std::string& name = "LED") 
        : RoviComponent(name)
        {
            handler[std::string("setPower")]        = std::bind(&LEDComponent::setPowerMQTT,        this, std::placeholders::_1);
            handler[std::string("setColor")]        = std::bind(&LEDComponent::setColorMQTT,        this, std::placeholders::_1);
            handler[std::string("setBrightness")]   = std::bind(&LEDComponent::setBrightnessMQTT,   this, std::placeholders::_1);
            handler[std::string("setEffect")]       = std::bind(&LEDComponent::setEffectMQTT,       this, std::placeholders::_1);
        };

    LEDComponent(const LEDComponent& other) = default;
    virtual ~LEDComponent() = default;

    // The convertion between string and concrete data type can be done in this abstract class
    // But the actual action have to be implemented in an derived class
    // Names due to overloaded function and bind make problems and would led to long typecasts...
    virtual void setPowerMQTT(const std::string& payload) {
        Serial << "  setPower() " << endl;

        // Convert to lower cases
        std::string localPayload = payload;
        std::transform(localPayload.begin(), localPayload.end(), localPayload.begin(),::tolower);

        bool power = false;
        if(payload == "on" || payload == "true") {
            power = true;
        }

        setPower(power);
    }

    virtual void setColorMQTT(const std::string& payload) {
        Serial << "  setColor() " << endl;
        rgba = RGBAColor(payload);

        Serial << "    Color: " << rgba.toString().c_str() << endl;

        setColor(rgba);
    }

    virtual void setBrightnessMQTT(const std::string& payload) {
        Serial << "  setBrightness() " << endl;

        uint8_t brightness = 255;
        // stoi is not supported by PlatformIO... :(
        // try {
        //     brightness = std::stoi(payload);
        // } catch(const std::invalid_argument e& e) {
        //     Serial << "  Payload (" << payload << ") could not be converted to uint8_t" << endl << e.what() << endl;
        // }

        // TODO: Error handling
        brightness = atoi(payload.c_str());

        setBrightness(brightness);        
    }

    virtual void setEffectMQTT(const std::string& payload) {
        Serial << "  setEffect() " << endl;
        Serial << "!!!! NOT IMPLEMENTED YET !!!!" << endl;
        // TODO

        t = std::thread(&LEDComponent::colorFlow, this);

        // t.join();

    }

    void colorFlow() {
        for(double h = 0.0; h < 255.0; ++h) {
            hsv hsvValue;
            hsvValue.h = h;
            hsvValue.s = 1.0;
            hsvValue.v = 0.5;

            rgb rgbValue = hsv2rgb(hsvValue);

            RGBAColor rgba(rgbValue.r * 255, rgbValue.g * 255, rgbValue.b * 255);
            setColor(rgba);
            delay(100);
        }

        Serial << "colorFlow finished" << endl;
    }


    // Interfac for the derived class
    virtual void setColor(const RGBAColor& rgba) = 0;
    virtual void setPower(bool power) = 0;
    virtual void setBrightness(uint8_t brightness) = 0;
    // virtual void setEffect(something) = 0;

// protected:
    RGBAColor rgba;

    std::thread t;
};

#endif /* __LEDCOMPONENT_H__ */