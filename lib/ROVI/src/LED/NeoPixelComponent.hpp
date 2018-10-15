#ifndef __NEOPIXELCOMPONENT_H__
#define __NEOPIXELCOMPONENT_H__

#include <Arduino.h>

#include <string>

#include <ArduinoIostream.hpp>

#include <Adafruit_NeoPixel.h>

#include "BaseClasses/RoviComponent.hpp"
#include "LED/LEDComponent.hpp"
#include "LED/ColorTypes.h"
#include "LEDEffect.hpp"

class NeoPixelComponent : public LEDComponent /*, public std::enable_shared_from_this<NeoPixelComponent> */ {
public:
    NeoPixelComponent(uint16_t nbPixel, uint8_t pin, const std::string& name = "NeoPixel") 
        : LEDComponent(name), pixels(nbPixel, pin, NEO_GRB + NEO_KHZ800)        // TBD: Maybe make the Pixeltype dynamic as well
        {
            pixels.begin();                                                     // This initializes the NeoPixel library.
                                                                                // TBD: Must this be called by the setup method
                                                                                //      as for Basecamp?

            setPower(true);
        };

    NeoPixelComponent(const NeoPixelComponent& other) = default;
    virtual ~NeoPixelComponent() = default;


    virtual void setColor(const std::shared_ptr<Color>& color) override {
        Serial << "--- NeoPixel::setColor" << endl;
        powerStatus = true;

        std::shared_ptr<RGBColor> rgb = color->toRGB();

        for(uint16_t pixelIdx = 0; pixelIdx < pixels.numPixels(); ++pixelIdx) {
            pixels.setPixelColor(pixelIdx, rgb->r, rgb->g, rgb->b);
        }
        pixels.show();

        lastColor = rgb;

        Serial << "   lastColor: " << lastColor->toString() << endl;
    }

    virtual void setPower(bool power) override {
        Serial << "--- NeoPixel::setPower" << endl;
        if(power) {
            setColor(lastColor);
        } else {
            std::shared_ptr<Color> tmpLastColor = lastColor;
            setColor(std::make_shared<RGBColor>(0,0,0));
            lastColor = tmpLastColor;
        }

        powerStatus = power;
    };

    virtual void setBrightness(uint8_t brightness) override {
        Serial << "--- NeoPixel::setBrightness" << endl;
        powerStatus = true;

        for(uint16_t pixelIdx = 0; pixelIdx < pixels.numPixels(); ++pixelIdx) {
            pixels.setBrightness(brightness);
        }
        pixels.show();

        // TBD: Should the new color be stored?
        // If so, convert NeoPixels color type...
    }


// protected:
    Adafruit_NeoPixel pixels;
};

#endif /* __NEOPIXELCOMPONENT_H__ */