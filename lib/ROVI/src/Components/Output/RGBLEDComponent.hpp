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
                m_pinR(pinR), m_pinG(pinG), m_pinB(pinB), 
                m_invert(invert), m_ledArray{1, 2, 3}, // three led channels
                {
                    ledcAttachPin(m_pinR, m_ledArray[0]); // assign RGB led pins to channels
                    ledcAttachPin(m_pinG, m_ledArray[1]);
                    ledcAttachPin(m_pinB, m_ledArray[2]);

                    // Initialize channels 
                    // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
                    // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
                    ledcSetup(ledArray[0], 12000, 8); // 12 kHz PWM, 8-bit resolution
                    ledcSetup(ledArray[1], 12000, 8);
                    ledcSetup(ledArray[2], 12000, 8);

                    setOn(true);
                };

            RGBLEDComponent(const RGBLEDComponent& other) = default;
            virtual ~RGBLEDComponent() = default;


            virtual void setColor(const std::shared_ptr<Color>& color) override {
                Serial << "--- LEDRGB::setColor" << endl;
                m_on = true;

                // dynamic_pointer_cast requires RTTI...
                // auto rgb = std::dynamic_pointer_cast<RGBColor>(color);
                auto rgb = color->toRGB();

                // write the RGB values to the pins
                ledcWrite(m_ledArray[0], rgb->r); // write red component to channel 1, etc.
                ledcWrite(m_ledArray[1], rgb->g);   
                ledcWrite(m_ledArray[2], rgb->b); 

                m_color = rgb;
            }

            virtual void setOn(bool power) override {
                Serial << "--- LEDRGB::setPower" << endl;
                if(power) {
                    setColor(m_color);
                } else {
                    auto tmpLastColor = m_color;
                    setColor(std::make_shared<RGBColor>(0,0,0));
                    m_color = tmpLastColor;
                }

                m_on = power;
            };

            virtual void setBrightness(uint8_t brightness) override {
                Serial << "--- LEDRGB::setBrightness" << endl;
                m_power = true;

                auto hsvColor = lastColor->toHSV();
                hsvColor->v = brightness;

                setColor(hsvColor);
            }


        protected:
            uint8_t m_pinR;
            uint8_t m_pinG;
            uint8_t m_pinB;

            boolean m_invert;
            uint8_t m_ledArray[3] = {1, 2, 3}; // three led channels
        };
    }
}

#endif /* __RGBLEDCOMPONENT_H__ */