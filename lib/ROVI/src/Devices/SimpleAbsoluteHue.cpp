#include "SimpleAbsoluteHue.h"

#include <ArduinoIostream.hpp>
#include <UtilFunctions.hpp>

#include "Common/LED/LEDEffectFactory.hpp"

namespace Rovi {
    namespace Devices {
        SimpleAbsoluteHue::SimpleAbsoluteHue(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton,
        const uint8_t neoPixelPin, const uint16_t nbNeoPixelLEDs)
        : rotary{std::make_shared<Components::RotaryEncoderWithButton>(pinA, pinB, pinButton)},
          leds{std::make_shared<Components::NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin)},
          m_on{true},
          m_brightness{128},
          m_color{std::make_shared<HSVColor>(0.0f, 0.0f, 0.5f)},
          m_effect{LEDEffectFactory::getEffect("white_static", leds.get())} {
            setOn(m_on);
            setBrightness(m_brightness);
            setColor(m_color);
            setEffect(m_effect);

            setupNormal();
            setupClick();
            setupDoubleClick();
            setupHold();
        };

        void SimpleAbsoluteHue::update() {
            rotary->update();
            leds->update();
        }

        void SimpleAbsoluteHue::setupNormal() {
            auto activatedCallback = [this]() {
                std::cout << "NORMAL state activation callback" << endl;
                if(m_on) {
                    blink();
                }
            };

            auto valueChangeCallback = [this](int brightness) {
                std::cout << "NORMAL value change callback - New value = " << brightness << endl;
                setBrightness(brightness);
                setColor(m_color);

                // // TEST: Still required?
                // auto lastColor = leds->getLastColor();
                // auto color = lastColor->toHSV();
                // color->v = (float) brightness / 255.0f;
                // leds->setColor(color);    // Required, because otherwise the color is not restored when changen from brightness 0 to 1...
                //                         // TODO: Check, if this is also required and/or working for the RGB_LEDs
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = 255;
            const auto increment = 10;
            const auto preventOverflow = true;
            rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::NORMAL, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        }


        void SimpleAbsoluteHue::setupClick() {
            auto activatedCallback = [this]() {
                std::cout << "CLICKED state activation callback" << endl;
                setOn(!m_on);
            };

            auto valueChangeCallback = [&](int value) {
                std::cout << "CLICKED value change callback - Do nothing" << endl;
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = 10;
            const auto increment = 1;
            const auto preventOverflow = false;
            rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        }


        void SimpleAbsoluteHue::setupDoubleClick() {
            auto activatedCallback = [this]() {
                std::cout << "DOUBLE_CLICKED state activation callback" << endl;
                // leds->stopEffect();
                auto effect = LEDEffectFactory::getEffect("color_static", leds.get());
                setEffect(effect);   
                doubleBlink();
            };

            auto valueChangeCallback = [&](int hue) {
                std::cout << "DOUBLE_CLICKED value change callback - New value = " << hue << endl;
                setHue(hue);
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = 360;
            const auto increment = 10;
            const auto preventOverflow = false;
            rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        }


        void SimpleAbsoluteHue::setupHold() {
            auto activatedCallback = [this]() {
                std::cout << "HOLDED state activation callback" << endl;
                leds->stopEffect();
                blink(500);
            };

            auto valueChangeCallback = [&](int effectNumber) {
                std::cout << "HOLDED value change callback - New value = " << effectNumber << endl;
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

        bool SimpleAbsoluteHue::on() const {
            return m_on;
        }

        void SimpleAbsoluteHue::setOn(const bool on) {
            m_on = on;
            if(m_on) {
                leds->startEffect();
            } else {
                leds->stopEffect();
            }
            leds->setOn(m_on);
        }

        uint8_t SimpleAbsoluteHue::brightness() const {
            return m_brightness;
        }

        void SimpleAbsoluteHue::setBrightness(const int brightness) {
            setOn(true);
            auto limitedBrightness = Utils::clamp(brightness, 0, 255);      // TBD: Move to Color class?
            m_brightness = limitedBrightness;
            leds->setBrightness(m_brightness);
        }

        std::shared_ptr<Color> SimpleAbsoluteHue::color() const {
            return m_color;
        }

        void SimpleAbsoluteHue::setColor(const std::shared_ptr<Color>& color) {
            setOn(true);
            m_color = color;
            leds->setColor(m_color);
        }

        float SimpleAbsoluteHue::hue() const {
            return m_color->toHSV()->h;
        }

        void SimpleAbsoluteHue::setHue(float hue) {
            setOn(true);
            auto hsvColor = m_color->toHSV();
            hsvColor->h = Utils::clamp(hue, 0.0f, 360.0f);      // TODO: Move to Color class
            setColor(hsvColor);
        }

        std::shared_ptr<LEDEffect> SimpleAbsoluteHue::setEffect() const {
            return m_effect;
        }

        void SimpleAbsoluteHue::setEffect(int effectNumber) {
            auto effectName = LEDEffectFactory::convertEffectNumberToName(effectNumber);
            setEffect(effectName);      
        }

        void SimpleAbsoluteHue::setEffect(const std::string& effectName) {
            auto effect = LEDEffectFactory::getEffect(effectName, leds.get());
            setEffect(effect);                
        }

        void SimpleAbsoluteHue::setEffect(const std::shared_ptr<LEDEffect>& effect) {
            setOn(true);
            m_effect = effect;
            leds->setEffect(m_effect); 
            leds->startEffect();    // TBD: Required?     
        }
    }
}