#include "SimpleAbsoluteHue.h"

#include <ArduinoIostream.hpp>

#include "Common/LED/LEDEffectFactory.hpp"

namespace Rovi {
    namespace Devices {
        SimpleAbsoluteHue::SimpleAbsoluteHue(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton,
        const uint8_t neoPixelPin, const uint16_t nbNeoPixelLEDs)
        : rotary{std::make_shared<Components::RotaryEncoderWithButton>(pinA, pinB, pinButton)}
        , leds{std::make_shared<Components::NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin)}
        , m_on{true}
        , m_brightness{50}
        , m_color{std::make_shared<HSVColor>(0.0f, 0.0f, 0.5f)}
        , m_effect{LEDEffectFactory::getEffect("white_static", leds.get())} 
        {
            setOn(m_on);
            setBrightness(m_brightness);
            setColor(m_color);
            setEffect(m_effect);

            setupNormal();
            setupClick();
            setupDoubleClick();
            setupHold();

            // TODO: Add possible effects list
        };

        void SimpleAbsoluteHue::update() {
            rotary->update();
            leds->update();
        }

        void SimpleAbsoluteHue::setupNormal() {
            auto activatedCallback = [this]() {
                std::cout << "NORMAL state activation callback - Brightness selection active" << std::endl;
                if(m_on) {
                    std::cout << "--- blink()" << std::endl;
                    blink();
                } else {
                    std::cout << "--- power off --> do nothing" << std::endl;
                }
            };

            auto valueChangeCallback = [this](int brightness) {
                std::cout << "NORMAL value change callback - Set brightness to new value = " << brightness << std::endl;
                setBrightness(brightness);
                // Not sure why this is required but otherwise the brightness is not set at all when changing
                // from 0 to a small percentage
                if(brightness < 20) {
                    setBrightness(brightness);
                }
                std::cout << "--- brightness set to " << (uint32_t) m_brightness << std::endl;
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = 100;
            const auto increment = 10;
            const auto preventOverflow = true;
            rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::NORMAL, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
            rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::NORMAL, m_brightness);
        }


        void SimpleAbsoluteHue::setupClick() {
            auto activatedCallback = [this]() {
                std::cout << "CLICKED state activation callback - Toggle power" << std::endl;
                setOn(!m_on);
            };

            auto valueChangeCallback = [&](int value) {
                std::cout << "CLICKED value change callback - Do nothing" << std::endl;
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = 10;
            const auto increment = 1;
            const auto preventOverflow = false;
            rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        }


        void SimpleAbsoluteHue::setupDoubleClick() {
            auto activatedCallback = [this]() {
                std::cout << "DOUBLE_CLICKED state activation callback - Hue selection active" << std::endl;
                std::cout << "--- Set effect to color_static and double blink" << std::endl;

                auto effect = LEDEffectFactory::getEffect("color_static", leds.get());
                setEffect(effect);   
                doubleBlink();
            };

            auto valueChangeCallback = [&](int hue) {
                std::cout << "DOUBLE_CLICKED value change callback - Set hue to new value = " << hue << std::endl;
                auto color = std::make_shared<HSVColor>(hue, 1.0f, 1.0f);
                setColor(color);
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = 360;
            const auto increment = 10;
            const auto preventOverflow = false;
            rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        }


        void SimpleAbsoluteHue::setupHold() {
            auto activatedCallback = [this]() {
                std::cout << "HOLDED state activation callback - Effect selectio  active" << std::endl;
                std::cout << "--- Stop current effect and do a long blink" << std::endl;
                leds->stopEffect();
                blink(500);
            };

            auto valueChangeCallback = [&](int effectNumber) {
                std::cout << "HOLDED value change callback - Select effect number " << effectNumber << ", i.e. effect '" << LEDEffectFactory::convertEffectNumberToName(effectNumber) << "'" << std::endl;
                setEffect(effectNumber);
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = LEDEffectFactory::getNumberOfEffects() - 1;;
            const auto increment = 1;
            const auto preventOverflow = false;
            rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::HOLDED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        }


        void SimpleAbsoluteHue::blink(const uint32_t delay_ms) {
            leds->setOn(false);
            delay(delay_ms);
            leds->setOn(true);
        }

        void SimpleAbsoluteHue::doubleBlink(const uint32_t delay_ms) {
            blink(delay_ms);
            delay(delay_ms);
            blink(delay_ms);
        }

        bool SimpleAbsoluteHue::isOn() const {
            return m_on;
        }

        void SimpleAbsoluteHue::setOn(bool on) {
            leds->setOn(on);
            m_on = leds->isOn();
        }

        uint8_t SimpleAbsoluteHue::brightness() const {
            return m_brightness;
        }

        void SimpleAbsoluteHue::setBrightness(uint8_t brightness) {
            setOn(true);
            leds->setBrightness(brightness);
            m_brightness = leds->brightness();
            rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::NORMAL, m_brightness);
        }

        std::shared_ptr<Color> SimpleAbsoluteHue::color() const {
            return m_color;
        }

        void SimpleAbsoluteHue::setColor(const std::shared_ptr<Color>& color) {
            leds->setColor(color);
            m_color = leds->color();
            rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, m_color->toHSV()->h);
        }

        uint32_t SimpleAbsoluteHue::hue() const {
            return leds->hue();
        }

        void SimpleAbsoluteHue::setHue(uint32_t hue) {
            std::cout << "--- SimpleAbsoluteHue::setHue() to " << hue << std::endl;
            leds->setHue(hue);
            m_color = leds->color();
            rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, m_color->toHSV()->h);
        }

        std::shared_ptr<LEDEffect> SimpleAbsoluteHue::effect() const {
            return m_effect;
        }

        void SimpleAbsoluteHue::setEffect(const std::shared_ptr<LEDEffect>& effect) {
            leds->setEffect(effect); 
            m_effect = leds->effect();
        }

        void SimpleAbsoluteHue::setEffect(int effect) {
            leds->setEffect(effect); 
            m_effect = leds->effect();   
        }

        void SimpleAbsoluteHue::setSwapRGValues(const std::vector<uint32_t> newSwapRGValues) {
            leds->setSwapRGValues(newSwapRGValues);
        }
    }
}
