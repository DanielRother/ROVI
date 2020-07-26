#ifndef __SIMPLE_ABSOLUTE_HUE_H__
#define __SIMPLE_ABSOLUTE_HUE_H__

#include <memory>

#include "SimpleLedDevice.hpp"
#include "Components/Input/RotaryEncoderWithButton.hpp"
#include "Common/LED/ColorTypes.h"

namespace Rovi {
    namespace Devices {
        template<class ROTARY, class LED>
        class SimpleAbsoluteHue : public SimpleLedDevice<LED> {
            public:
                SimpleAbsoluteHue(Basecamp& iot, const std::shared_ptr<ROTARY> rotary, 
                    const std::shared_ptr<LED> led, const std::vector<std::shared_ptr<Rovi::LEDEffect>> effects, 
                    const std::string& name = "SimpleAbsoluteHue", std::chrono::minutes timePerEffect = std::chrono::minutes{0}) 
                : BasicDevice(iot)
                , SimpleLedDevice<LED>(iot, led, effects, name, timePerEffect)
                , m_rotary{rotary}
                {
                    // Attention: Initialize the rotary encoder first. Otherwise the led-set-methods
                    // wil fail due to unint rotary states
                    std::cout << "SimpleAbsoluteHue(): Init rotary" << std::endl;
                    setupNormal();
                    setupClick();
                    setupDoubleClick();
                    setupHold();

                    std::cout << "SimpleAbsoluteHue(): Init led" << std::endl;
                    // Attention: Do NOT init the leds here again. Otherwise the application hangs.:!
                }
                                
                virtual void update() {
                    m_rotary->update();
                    SimpleLedDevice<LED>::update();
                }

                virtual void setBrightness(uint8_t brightness) {
                    std::cout << "SimpleAbsoluteHue()::setBrightness(" << (int) brightness << ")" << std::endl;
                    SimpleLedDevice<LED>::setBrightness(brightness);
                    m_rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::NORMAL, this->m_brightness);
                }

                virtual void setColor(const std::shared_ptr<Color>& color) {
                    std::cout << "SimpleAbsoluteHue()::setColor(" << color->toString() << ")" << std::endl;
                    SimpleLedDevice<LED>::setColor(color);
                    m_rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, this->m_color->toHSV()->h);
                }

                virtual void setHue(uint32_t hue) {
                    std::cout << "SimpleAbsoluteHue()::setHue(" << hue << ")" << std::endl;
                    SimpleLedDevice<LED>::setHue(hue);
                    m_rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, this->m_color->toHSV()->h);
                }

                virtual void setEffect(const std::shared_ptr<LEDEffect>& effect) {
                    std::cout << "SimpleAbsoluteHue()::setEffect(" << effect->name() << ")" << std::endl;
                    SimpleLedDevice<LED>::setEffect(effect);
                    for(auto effectIdx = 0; effectIdx < this->m_effects.size(); ++effectIdx) {
                        if(this->m_effects[effectIdx]->name() == effect->name()) {
                            m_rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::HOLDED, effectIdx);
                            break;
                        }
                    }
                }

                virtual void setEffect(int effect) {
                    std::cout << "SimpleAbsoluteHue()::setEffect(" << effect << ")" << std::endl;
                    SimpleLedDevice<LED>::setEffect(effect);
                    m_rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::HOLDED, effect);                    
                }

            protected:
                void setupNormal() {
                    std::cout << "SimpleAbsoluteHue()::setupNormal()" << std::endl;
                    auto activatedCallback = [this]() {
                        std::cout << "NORMAL state activation callback - Brightness selection active" << std::endl;
                        if(this->m_on) {
                            std::cout << "--- blink()" << std::endl;
                            blink();
                        } else {
                            std::cout << "--- power off --> do nothing" << std::endl;
                        }
                    };

                    auto valueChangeCallback = [this](int brightness) {
                        std::cout << "NORMAL value change callback - Set brightness to new value = " << brightness << std::endl;
                        this->setBrightness(brightness);
                        std::cout << "--- brightness set to " << (uint32_t) this->m_brightness << std::endl;
                    };

                    const auto minRotaryValue = 0;
                    const auto maxRotaryValue = 100;
                    const auto increment = 10;
                    const auto preventOverflow = true;
                    m_rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::NORMAL, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
                    m_rotary->setValue(Components::RotaryEncoderWithButton::ButtonStates::NORMAL, this->m_brightness);
                }

                void setupClick() {
                    std::cout << "SimpleAbsoluteHue()::setupClick()" << std::endl;
                    auto activatedCallback = [this]() {
                        std::cout << "CLICKED state activation callback - Toggle power" << std::endl;
                        this->setOn(!this->m_on);
                    };

                    auto valueChangeCallback = [&](int value) {
                        std::cout << "CLICKED value change callback - Do nothing" << std::endl;
                    };

                    const auto minRotaryValue = 0;
                    const auto maxRotaryValue = 10;
                    const auto increment = 1;
                    const auto preventOverflow = false;
                    m_rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
                }

                void setupDoubleClick() {
                    std::cout << "SimpleAbsoluteHue()::setupDoubleClick()" << std::endl;
                    auto activatedCallback = [this]() {
                        std::cout << "DOUBLE_CLICKED state activation callback - Hue selection active" << std::endl;
                        std::cout << "--- Set effect to color_static and double blink" << std::endl;

                        for(auto effectIdx = 0; effectIdx < this->m_effects.size(); ++effectIdx) {
                            if(this->m_effects[effectIdx]->name() == "color_static") {
                                this->setEffect(effectIdx);
                                break;
                            }
                        } 
                        doubleBlink();
                    };

                    auto valueChangeCallback = [&](int hue) {
                        std::cout << "DOUBLE_CLICKED value change callback - Set hue to new value = " << hue << std::endl;
                        auto color = std::make_shared<HSVColor>(hue, 1.0f, 1.0f);
                        this->setColor(color);
                    };

                    const auto minRotaryValue = 0;
                    const auto maxRotaryValue = 360;
                    const auto increment = 10;
                    const auto preventOverflow = false;
                    m_rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
                }

                void setupHold() {
                    std::cout << "SimpleAbsoluteHue()::setupHold()" << std::endl;
                    auto activatedCallback = [this]() {
                        std::cout << "HOLDED state activation callback - Effect selectio  active" << std::endl;
                        std::cout << "--- Stop current effect and do a long blink" << std::endl;
                        this->m_leds->stopEffect();
                        blink(500);
                    };

                    auto valueChangeCallback = [&](int effectNumber) {
                        std::cout << "HOLDED value change callback - Select effect number " << effectNumber << ", i.e. effect '" << this->m_effects[effectNumber]->name() << "'" << std::endl;
                        this->setEffect(effectNumber);
                    };

                    const auto minRotaryValue = 0;
                    const auto maxRotaryValue = this->m_effects.size() - 1;
                    const auto increment = 1;
                    const auto preventOverflow = false;
                    m_rotary->setupState(Components::RotaryEncoderWithButton::ButtonStates::HOLDED, minRotaryValue, maxRotaryValue, increment, preventOverflow, activatedCallback, valueChangeCallback);
                }

                void blink(const uint32_t delay_ms = 250) {
                    this->setOn(false);
                    delay(delay_ms);
                    this->setOn(true);
                }

                void doubleBlink(const uint32_t delay_ms = 250) {
                    blink(delay_ms);
                    delay(delay_ms);
                    blink(delay_ms);
                }

                std::shared_ptr<ROTARY> m_rotary;
        };
    }
}

#endif