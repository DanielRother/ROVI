#include "SimpleAbsoluteHue.h"

#include <ArduinoIostream.hpp>

#include "Common/LED/LEDEffectFactory.hpp"

namespace Rovi {
    namespace Devices {
        // template<class ROTARY, class LED>
        // SimpleAbsoluteHue<ROTARY, LED>::SimpleAbsoluteHue(const std::shared_ptr<ROTARY> rotary, 
        //     const std::shared_ptr<LED> led, const std::vector<std::shared_ptr<Rovi::LEDEffect>> effects, 
        //     const std::string& name, std::chrono::minutes timePerEffect)
        // : SimpleLedDevice<LED>(led, effects, name, timePerEffect)
        // , m_rotary{rotary}
        // {
        //     setOn(this->m_on);
        //     setBrightness(this->m_brightness);
        //     setColor(this->m_color);
        //     setEffect(this->m_effect);

        //     setupNormal();
        //     setupClick();
        //     setupDoubleClick();
        //     setupHold();
        // };

        // template<class ROTARY, class LED>
        // void SimpleAbsoluteHue<ROTARY, LED>::update() {
        //     // m_rotary->update();
        //     // SimpleLedDevice<LED>::update();
        //     // // leds->update();
        // }

        // template<class ROTARY, class LED>
        // void SimpleAbsoluteHue<ROTARY, LED>::setupNormal() {
        //     auto activatedCallback = [this]() {
        //         std::cout << "NORMAL state activation callback - Brightness selection active" << std::endl;
        //         if(this->m_on) {
        //             std::cout << "--- blink()" << std::endl;
        //             blink();
        //         } else {
        //             std::cout << "--- power off --> do nothing" << std::endl;
        //         }
        //     };

        //     auto valueChangeCallback = [this](int brightness) {
        //         std::cout << "NORMAL value change callback - Set brightness to new value = " << brightness << std::endl;
        //         this->setBrightness(brightness);
        //         std::cout << "--- brightness set to " << (uint32_t) this->m_brightness << std::endl;
        //     };

        //     const auto minRotaryValue = 0;
        //     const auto maxRotaryValue = 100;
        //     const auto increment = 10;
        //     const auto preventOverflow = true;
        //     m_rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::NORMAL, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        //     m_rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::NORMAL, this->m_brightness);
        // }


        // template<class ROTARY, class LED>
        // void SimpleAbsoluteHue<ROTARY, LED>::setupClick() {
        //     auto activatedCallback = [this]() {
        //         std::cout << "CLICKED state activation callback - Toggle power" << std::endl;
        //         setOn(!this->m_on);
        //     };

        //     auto valueChangeCallback = [&](int value) {
        //         std::cout << "CLICKED value change callback - Do nothing" << std::endl;
        //     };

        //     const auto minRotaryValue = 0;
        //     const auto maxRotaryValue = 10;
        //     const auto increment = 1;
        //     const auto preventOverflow = false;
        //     m_rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        // }


        // template<class ROTARY, class LED>
        // void SimpleAbsoluteHue<ROTARY, LED>::setupDoubleClick() {
        //     auto activatedCallback = [this]() {
        //         std::cout << "DOUBLE_CLICKED state activation callback - Hue selection active" << std::endl;
        //         std::cout << "--- Set effect to color_static and double blink" << std::endl;

        //         // TODO?
        //         auto effect = LEDEffectFactory::getEffect("color_static", this->m_leds.get());
        //         this->setEffect(effect);   
        //         doubleBlink();
        //     };

        //     auto valueChangeCallback = [&](int hue) {
        //         std::cout << "DOUBLE_CLICKED value change callback - Set hue to new value = " << hue << std::endl;
        //         auto color = std::make_shared<HSVColor>(hue, 1.0f, 1.0f);
        //         this->setColor(color);
        //     };

        //     const auto minRotaryValue = 0;
        //     const auto maxRotaryValue = 360;
        //     const auto increment = 10;
        //     const auto preventOverflow = false;
        //     m_rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        // }


        // template<class ROTARY, class LED>
        // void SimpleAbsoluteHue<ROTARY, LED>::setupHold() {
        //     auto activatedCallback = [this]() {
        //         std::cout << "HOLDED state activation callback - Effect selectio  active" << std::endl;
        //         std::cout << "--- Stop current effect and do a long blink" << std::endl;
        //         this->m_leds->stopEffect();
        //         blink(500);
        //     };

        //     auto valueChangeCallback = [&](int effectNumber) {
        //         std::cout << "HOLDED value change callback - Select effect number " << effectNumber << ", i.e. effect '" << this->m_effects[effectNumber]->name() << "'" << std::endl;
        //        // TODO?
        //        this->setEffect(LEDEffectFactory::getEffect(this->m_effects[effectNumber], this->m_leds.get()));
        //     };

        //     const auto minRotaryValue = 0;
        //     const auto maxRotaryValue = this->m_effects.size() - 1;
        //     const auto increment = 1;
        //     const auto preventOverflow = false;
        //     m_rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::HOLDED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
        // }


        // template<class ROTARY, class LED>
        // void SimpleAbsoluteHue<ROTARY, LED>::blink(const uint32_t delay_ms) {
        //     this->setOn(false);
        //     delay(delay_ms);
        //     this->setOn(true);
        // }

        // template<class ROTARY, class LED>
        // void SimpleAbsoluteHue<ROTARY, LED>::doubleBlink(const uint32_t delay_ms) {
        //     blink(delay_ms);
        //     delay(delay_ms);
        //     blink(delay_ms);
        // }

        // bool SimpleAbsoluteHue::isOn() const {
        //     return m_on;
        // }

        // void SimpleAbsoluteHue::setOn(bool on) {
        //     leds->setOn(on);
        //     m_on = leds->isOn();
        // }

        // uint8_t SimpleAbsoluteHue::brightness() const {
        //     return m_brightness;
        // }

        // void SimpleAbsoluteHue::setBrightness(uint8_t brightness) {
        //     setOn(true);
        //     leds->setBrightness(brightness);
        //     // Not sure why this is required but otherwise the brightness is not set at all when changing
        //     // from 0 to a small percentage
        //     if(m_brightness == 0) {
        //         leds->setBrightness(brightness);
        //     }
        //     m_brightness = leds->brightness();
        //     rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::NORMAL, m_brightness);
        // }

        // std::shared_ptr<Color> SimpleAbsoluteHue::color() {
        //     m_color = leds->color();
        //     return m_color;
        // }

        // void SimpleAbsoluteHue::setColor(const std::shared_ptr<Color>& color) {
        //     auto effect = LEDEffectFactory::getEffect("color_static", leds.get());
        //     setEffect(effect);   

        //     leds->setColor(color);
        //     m_color = leds->color();
        //     rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, m_color->toHSV()->h);
        // }

        // uint32_t SimpleAbsoluteHue::hue() const {
        //     return leds->hue();
        // }

        // void SimpleAbsoluteHue::setHue(uint32_t hue) {
        //     std::cout << "--- SimpleAbsoluteHue::setHue() to " << hue << std::endl;
        //     leds->setHue(hue);
        //     m_color = leds->color();
        //     rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, m_color->toHSV()->h);
        // }

        // std::shared_ptr<LEDEffect> SimpleAbsoluteHue::effect() const {
        //     return m_effect;
        // }

        // void SimpleAbsoluteHue::setEffect(const std::shared_ptr<LEDEffect>& effect) {
        //     leds->setEffect(effect); 
        //     m_effect = leds->effect();
        // }

        // void SimpleAbsoluteHue::setEffect(int effect) {
        //     leds->setEffect(effect); 
        //     m_effect = leds->effect();   
        // }

        // template<class ROTARY, class LED>
        // void SimpleAbsoluteHue<ROTARY, LED>::setSwapRGValues(const std::vector<uint32_t> newSwapRGValues) {
        //     this->m_leds->setSwapRGValues(newSwapRGValues);
        // }
    }
}
