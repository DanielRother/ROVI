#ifndef __SIMPLE_ABSOLUTE_HUE_H__
#define __SIMPLE_ABSOLUTE_HUE_H__

#include <memory>

#include "../Input/RotaryEncoderWithButton.hpp"     // TODO: Simplfy, i.e. remove MQTT
#include "../LED/ColorTypes.h"
#include "../LED/NeoPixelComponent.hpp"             // TODO: Simplfy, i.e. remove MQTT

namespace Rovi {
    namespace Devicec {
        class SimpleAbsoluteHue {
            public:
                SimpleAbsoluteHue(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton,
                                const uint8_t neoPixelPin, const uint16_t nbNeoPixelLEDs);

                bool on() const;
                void setOn(const bool on);
                uint8_t brightness() const;
                void setBrightness(const int brightness);
                std::shared_ptr<Color> color() const;
                void setColor(const std::shared_ptr<Color>& color);
                float hue() const;
                void setHue(float hue);
                std::shared_ptr<LEDEffect> setEffect() const;
                void setEffect(int effectNumber);
                void setEffect(const std::string& effectName);

            protected:
                void setupNormal();
                void setupClick();
                void setupDoubleClick();
                void setupHold();

                void blink(const uint32_t delay_ms = 250);
                void doubleBlink(const uint32_t delay_ms = 250);

                std::shared_ptr<RotaryEncoderWithButton> rotary;
                std::shared_ptr<NeoPixelComponent> leds;

                // TODO: Default values
                bool m_on;
                uint8_t m_brightness;
                std::shared_ptr<Color> m_color;
                std::shared_ptr<LEDEffect> m_effect;
        };
    }
}

#endif