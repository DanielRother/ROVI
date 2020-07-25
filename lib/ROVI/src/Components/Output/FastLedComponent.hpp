#ifndef __FASTLEDCOMPONENT_H__
#define __FASTLEDCOMPONENT_H__

#include <Arduino.h>

#include <string>
#include <vector>

#include <ArduinoIostream.hpp>

#include <FastLED.h>

#include "BaseClasses/RoviComponent.hpp"
#include "LEDComponent.hpp"
#include "Common/LED/ColorTypes.h"
#include "Common/LED/LEDEffect.hpp"

namespace Rovi {
    namespace Components {
        template<uint8_t PIN, std::size_t NB_PIXEL>
        class FastLedComponent : public AdressableLedComponent /*, public std::enable_shared_from_this<FastLedComponent> */ {
        public:
            FastLedComponent(const std::string& name = "FastLED") 
                : AdressableLedComponent{NB_PIXEL, name}
                , m_isInit{false}
                , m_leds{}
                {
                    setOn(true);
                };

            FastLedComponent(const FastLedComponent& other) = default;
            virtual ~FastLedComponent() = default;

            // TBD: Why must this be called later and from external and cannot be done in the ctor or inside of this class?!
            void init() {
                // FastLED.addLeds<NEOPIXEL, PIN>(m_leds.data(), NB_PIXEL);
                FastLED.addLeds<WS2811, PIN, RGB>(m_leds.data(), NB_PIXEL);      // TODO: Pass the ordering as ctor variable
                m_isInit = true;
            }

            virtual void setColorImpl(const std::shared_ptr<Color>& color) override {
                Serial << "--- FastLedComponent::setColor(): " << color->toString() << endl;

                if(!m_isInit) {
                    std::cout << "Not initialized!" << std::endl;
                    return;
                    // init();      // Not possible?!?!?!
                }

                auto rgb = color->toRGB();
                for(uint16_t pixelIdx = 0; pixelIdx < NB_PIXEL; ++pixelIdx) {
                    m_leds[pixelIdx] = CRGB(rgb->r, rgb->g, rgb->b); 
                }
                FastLED.show();
                m_color = color;
            }

            virtual void setColorImpl(const std::shared_ptr<Color>& color, size_t pixelIdx) override {
                // Serial << "--- FastLedComponent::setColor" << endl;

                AdressableLedComponent::setColorImpl(color, pixelIdx);

                if(!m_isInit) {
                    std::cout << "Not initialized!" << std::endl;
                    return;
                    // init();      // Not possible?!?!?!
                }

                auto rgb = color->toRGB();
                m_leds[pixelIdx] = CRGB(rgb->r, rgb->g, rgb->b);
                FastLED.show();
                m_color = color; 
            }

            virtual void setBrightnessImpl(uint8_t brightness) override {
                if(!m_isInit) {
                    std::cout << "Not initialized!" << std::endl;
                    return;
                    // init();      // Not possible?!?!?!
                }

                Serial << "--- FastLedComponent::setBrightness to " << (uint32_t) brightness << endl;
                FastLED.setBrightness(brightness);
                // for(uint16_t pixelIdx = 0; pixelIdx < NB_PIXEL; ++pixelIdx) {
                //     // m_leds[pixelIdx].fadeToBlackBy( brightness );
                // }
                FastLED.show();
            }

            virtual void showImpl() override {
                FastLED.show();
            }


        protected:
            bool m_isInit;
            std::array<CRGB, NB_PIXEL> m_leds;
        };
    }
}

#endif /* __FASTLEDCOMPONENT_H__ */