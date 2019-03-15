#ifndef __RGBLEDCOMPONENT_H__
#define __RGBLEDCOMPONENT_H__

#include <Arduino.h>

#include <string>

#include <ArduinoIostream.hpp>

#include "BaseClasses/RoviComponent.hpp"
#include "LED/LEDComponent.hpp"
#include "LED/ColorTypes.h"
#include "LEDEffect.hpp"

namespace Rovi {
    namespace Components {
        class RGBLEDComponent : public LEDComponent /*, public std::enable_shared_from_this<NeoPixelComponent> */ {
        public:
            RGBLEDComponent(uint8_t pinR, uint8_t pinG, uint8_t pinB, boolean invert = false, const std::string& name = "RGB_LED") 
                : LEDComponent(name),
                pinR(pinR), pinG(pinG), pinB(pinB), 
                invert(invert),ledArray{1, 2, 3}, // three led channels
                lastColor(std::make_shared<RGBColor>(128,128,128))
                {
                    // pinMode(pinR, OUTPUT);
                    // pinMode(pinG, OUTPUT);
                    // pinMode(pinB, OUTPUT);
                    ledcAttachPin(pinR, ledArray[0]); // assign RGB led pins to channels
                    ledcAttachPin(pinG, ledArray[1]);
                    ledcAttachPin(pinB, ledArray[2]);

                    // Initialize channels 
                    // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
                    // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
                    ledcSetup(ledArray[0], 12000, 8); // 12 kHz PWM, 8-bit resolution
                    ledcSetup(ledArray[1], 12000, 8);
                    ledcSetup(ledArray[2], 12000, 8);

                    setPower(true);
                };

            RGBLEDComponent(const RGBLEDComponent& other) = default;
            virtual ~RGBLEDComponent() = default;


            virtual void setColor(const std::shared_ptr<Color>& color) override {
                Serial << "--- LEDRGB::setColor" << endl;
                powerStatus = true;

                // dynamic_pointer_cast requires RTTI...
                // auto rgb = std::dynamic_pointer_cast<RGBColor>(color);
                std::shared_ptr<RGBColor> rgb = color->toRGB();

                // write the RGB values to the pins
                ledcWrite(ledArray[0], rgb->r); // write red component to channel 1, etc.
                ledcWrite(ledArray[1], rgb->g);   
                ledcWrite(ledArray[2], rgb->b); 

                lastColor = rgb;
            }

            virtual void setPower(bool power) override {
                Serial << "--- LEDRGB::setPower" << endl;
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
                Serial << "--- LEDRGB::setBrightness" << endl;
                powerStatus = true;

                auto hsvColor = lastColor->toHSV();
                hsvColor->v = brightness;

                setColor(hsvColor);
            }


        // protected:
            uint8_t pinR;
            uint8_t pinG;
            uint8_t pinB;

            boolean invert;
            uint8_t ledArray[3] = {1, 2, 3}; // three led channels
        };
    }
}

#endif /* __RGBLEDCOMPONENT_H__ */