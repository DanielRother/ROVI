#ifndef __NEOPIXELCOMPONENT_H__
#define __NEOPIXELCOMPONENT_H__

#include <Arduino.h>

#include <string>
#include <vector>

#include <ArduinoIostream.hpp>

#include <Adafruit_NeoPixel.h>

#include "BaseClasses/RoviComponent.hpp"
#include "LEDComponent.hpp"
#include "Common/LED/ColorTypes.h"
#include "Common/LED/LEDEffect.hpp"

namespace Rovi {
    namespace Components {
        class NeoPixelComponent : public AdressableLedComponent /*, public std::enable_shared_from_this<NeoPixelComponent> */ {
        public:
            NeoPixelComponent(uint16_t nbPixel, uint8_t pin, const std::string& name = "NeoPixel") 
                : AdressableLedComponent{nbPixel, name}
                , pixels{nbPixel, pin, NEO_GRB + NEO_KHZ800}
                , swapRGValues(nbPixel, 0)        // TBD: Maybe make the Pixeltype dynamic as well
                {
                    pixels.begin();                                                     // This initializes the NeoPixel library.
                                                                                        // TBD: Must this be called by the setup method
                                                                                        //      as for Basecamp?

                    setOn(true);
                };

            NeoPixelComponent(const NeoPixelComponent& other) = default;
            virtual ~NeoPixelComponent() = default;


            virtual void setColorImpl(const std::shared_ptr<Color>& color) override {
                Serial << "--- NeoPixel::setColor" << endl;
                auto rgb = color->toRGB();

                // pixels.setPixelColor(0,0,0,0);
                for(uint16_t pixelIdx = 0; pixelIdx < pixels.numPixels(); ++pixelIdx) {
                    // Serial << "pixelIdx: " << pixelIdx;
                    if(swapRGValues[pixelIdx] == 0) {
                        // Serial << " RGB" << endl;
                        pixels.setPixelColor(pixelIdx, rgb->r, rgb->g, rgb->b);
                    } else {
                        // Serial << " GRB" << endl;
                        pixels.setPixelColor(pixelIdx, rgb->g, rgb->r, rgb->b);
                    }
                }

                // Possible workaround to prevent flickering
                // See https://github.com/adafruit/Adafruit_NeoPixel/issues/139 for more details
                delay(1);
                pixels.show();
            }

            virtual void setColorImpl(const std::shared_ptr<Color>& color, size_t pixelIdx) override {
                Serial << "--- NeoPixel::setColor" << endl;
                AdressableLedComponent::setColorImpl(color, pixelIdx);

                auto rgb = color->toRGB();

                // Serial << "pixelIdx: " << pixelIdx;
                if(swapRGValues[pixelIdx] == 0) {
                    // Serial << " RGB" << endl;
                    pixels.setPixelColor(pixelIdx, rgb->r, rgb->g, rgb->b);
                } else {
                    // Serial << " GRB" << endl;
                    pixels.setPixelColor(pixelIdx, rgb->g, rgb->r, rgb->b);
                }
            }

            virtual void setBrightnessImpl(uint8_t brightness) override {
                Serial << "--- NeoPixel::setBrightness to " << (uint32_t) brightness << endl;
                pixels.setBrightness(brightness);
                // TBD: Should the new color be stored?
                // If so, convert NeoPixels color type...
            }

            virtual void showImpl() override {
                pixels.show();
            }

            void setSwapRGValues(const std::vector<uint32_t> newSwapRGValues) {
                swapRGValues = newSwapRGValues;
            }

        protected:
            Adafruit_NeoPixel pixels;
            std::vector<uint32_t> swapRGValues;
        };
    }
}

#endif /* __NEOPIXELCOMPONENT_H__ */