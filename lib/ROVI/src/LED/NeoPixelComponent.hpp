#ifndef __NEOPIXELCOMPONENT_H__
#define __NEOPIXELCOMPONENT_H__

#include <Arduino.h>

#include <string>

#include <ArduinoIostream.hpp>

#include <Adafruit_NeoPixel.h>

#include "BaseClasses/RoviComponent.hpp"
#include "LED/LEDComponent.hpp"
#include "LED/ColorTypes.h"

class NeoPixelComponent : public LEDComponent {
public:
    NeoPixelComponent(uint16_t nbPixel, uint8_t pin, const std::string& name = "NeoPixel") 
        : LEDComponent(name), pixels(nbPixel, pin, NEO_GRB + NEO_KHZ800),        // TBD: Maybe make the Pixeltype dynamic as well
          lastColor(128,128,128)
        {
            pixels.begin();                                                     // This initializes the NeoPixel library.
                                                                                // TBD: Must this be called by the setup method
                                                                                //      as for Basecamp?

            setPower(true);
        };

    NeoPixelComponent(const NeoPixelComponent& other) = default;
    virtual ~NeoPixelComponent() = default;


    virtual void setColor(const RGBAColor& rgba) override {
        Serial << "--- NeoPixel::setColor" << endl;

        for(uint16_t pixelIdx = 0; pixelIdx < pixels.numPixels(); ++pixelIdx) {
            pixels.setPixelColor(pixelIdx, rgba.R, rgba.G, rgba.B);
        }
        pixels.show();

        lastColor = rgba;
    }

    virtual void setPower(bool power) override {
        Serial << "--- NeoPixel::setPower" << endl;

        if(power) {
            setColor(lastColor);
        } else {
            RGBAColor tmpLastColor = lastColor;
            setColor(RGBAColor(0,0,0));
            lastColor = tmpLastColor;
        }
    };

    virtual void setBrightness(uint8_t brightness) override {
        Serial << "--- NeoPixel::setBrightness" << endl;

        for(uint16_t pixelIdx = 0; pixelIdx < pixels.numPixels(); ++pixelIdx) {
            pixels.setBrightness(brightness);
        }
        pixels.show();

        // TBD: Should the new color be stored?
        // If so, convert NeoPixels color type...
    }


// protected:
    Adafruit_NeoPixel pixels;
    RGBAColor lastColor;
};

#endif /* __NEOPIXELCOMPONENT_H__ */