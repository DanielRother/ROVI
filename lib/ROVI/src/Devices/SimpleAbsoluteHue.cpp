#include "SimpleAbsoluteHue.h"

#include <ArduinoIostream.hpp>
#include <UtilFunctions.hpp>

#include "../LED/LEDEffectFactory.hpp"

namespace Rovi {
    namespace Devicec {
        SimpleAbsoluteHue::SimpleAbsoluteHue(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton,
        const uint8_t neoPixelPin, const uint16_t nbNeoPixelLEDs)
        : rotary{std::make_shared<RotaryEncoderWithButton>(pinA, pinB, pinButton)},
        leds{std::make_shared<NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin)} {

            setupNormal();
            setupClick();
            setupDoubleClick();
            setupHold();
        };


        void SimpleAbsoluteHue::setupNormal() {
            auto activatedCallback = [this]() {
                Serial << "NORMAL state activation callback" << endl;
                if(m_on) {
                    blink();
                }
            };

            auto valueChangeCallback = [this](int brightness) {
                Serial << "NORMAL value change callback - New value = " << brightness << endl;
                setOn(true);
                setBrightness(brightness);

                // TEST: Still required?
                auto lastColor = leds->getLastColor();
                auto color = lastColor->toHSV();
                color->v = (float) brightness / 255.0f;
                leds->setColor(color);    // Required, because otherwise the color is not restored when changen from brightness 0 to 1...
                                        // TODO: Check, if this is also required and/or working for the RGB_LEDs
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = 255;
            const auto increment = 10;
            const auto preventOverflow = true;
            rotary->setupState(RotaryEncoderWithButton::ButtonStates::NORMAL, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        }


        void SimpleAbsoluteHue::setupClick() {
            auto activatedCallback = [this]() {
                Serial << "CLICKED state activation callback" << endl;
                setOn(!m_on);
            };

            auto valueChangeCallback = [&](int value) {
                Serial << "CLICKED value change callback - Do nothing" << endl;
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = 10;
            const auto increment = 1;
            const auto preventOverflow = false;
            rotary->setupState(RotaryEncoderWithButton::ButtonStates::CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        }


        void SimpleAbsoluteHue::setupDoubleClick() {
            auto activatedCallback = [this]() {
                Serial << "DOUBLE_CLICKED state activation callback" << endl;

                setOn(true);
                leds->stopEffect();
                // TODO: setEffect Color
                doubleBlink();
            };

            auto valueChangeCallback = [&](int hue) {
                Serial << "DOUBLE_CLICKED value change callback - New value = " << hue << endl;
                setHue(hue);   // TODO: getCurrentBrightness()?
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = 360;
            const auto increment = 10;
            const auto preventOverflow = false;
            rotary->setupState(RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        }


        void SimpleAbsoluteHue::setupHold() {
            auto activatedCallback = [this]() {
                Serial << "HOLDED state activation callback" << endl;
                setOn(true);
                leds->stopEffect();
                blink(500);
            };

            auto valueChangeCallback = [&](int effectNumber) {
                Serial << "HOLDED value change callback - New value = " << effectNumber << endl;
                setEffect(effectNumber);
            };

            const auto minRotaryValue = 0;
            const auto maxRotaryValue = LEDEffectFactory::getNumberOfEffects() - 1;;
            const auto increment = 1;
            const auto preventOverflow = false;
            rotary->setupState(RotaryEncoderWithButton::ButtonStates::HOLDED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        }


        void SimpleAbsoluteHue::blink(const uint32_t delay_ms) {
            leds->setPower(false);
            delay(delay_ms);
            leds->setPower(true);
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
            leds->setPower(m_on);
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
            setOn(true);
            m_effect = LEDEffectFactory::getEffect(effectName, leds.get());    
            leds->setEffect(m_effect); 
            leds->startEffect();    // TBD: Required?                 
        }
    }
}
