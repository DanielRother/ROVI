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
          lastColor(std::make_shared<RGBColor>(128,128,128))
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

        // dynamic_pointer_cast requires RTTI...
        // auto rgb = std::dynamic_pointer_cast<RGBColor>(color);
        std::shared_ptr<RGBColor> rgb = color->toRGB();

        for(uint16_t pixelIdx = 0; pixelIdx < pixels.numPixels(); ++pixelIdx) {
            pixels.setPixelColor(pixelIdx, rgb->r, rgb->g, rgb->b);
        }
        pixels.show();

        lastColor = rgb;
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
    std::shared_ptr<Color> lastColor;
};

#endif /* __NEOPIXELCOMPONENT_H__ */