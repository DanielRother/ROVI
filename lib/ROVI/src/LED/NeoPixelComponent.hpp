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
        : LEDComponent(name), pixels(nbPixel, pin, NEO_GRB + NEO_KHZ800)        // TBD: Maybe make the Pixeltype dynamic as well
        {
              pixels.begin();                                                    // This initializes the NeoPixel library.
                                                                                // TBD: Must this be called by the setup method
                                                                                //      As for Basecamp?
        };

    NeoPixelComponent(const NeoPixelComponent& other) = default;
    virtual ~NeoPixelComponent() = default;


    virtual void updateColor(const RGBAColor& rgba) override {
        for(uint16_t pixelIdx = 0; pixelIdx < pixels.numPixels(); ++pixelIdx) {
            pixels.setPixelColor(pixelIdx, rgba.R, rgba.G, rgba.B);
        }
        pixels.show();
    }

// protected:
    Adafruit_NeoPixel pixels;
};

#endif /* __NEOPIXELCOMPONENT_H__ */