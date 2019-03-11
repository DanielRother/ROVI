#ifndef __SIMPLE_ABSOLUTE_HUE_H__
#define __SIMPLE_ABSOLUTE_HUE_H__

#include <memory>

#include "../Input/RotaryEncoderWithButton.hpp"     // TODO: Simplfy, i.e. remove MQTT
#include "../LED/NeoPixelComponent.hpp"             // TODO: Simplfy, i.e. remove MQTT

class SimpleAbsoluteHue {
    public:
        SimpleAbsoluteHue(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton,
                          const uint8_t neoPixelPin, const uint16_t nbNeoPixelLEDs);

    protected:
        void setupNormal();
        void setupDoubleClick();
        void setupHold();

        void blink();

        std::shared_ptr<RotaryEncoderWithButton> rotary;
        std::shared_ptr<NeoPixelComponent> leds;

        bool m_on;
        uint8_t m_brightness;
};

#endif