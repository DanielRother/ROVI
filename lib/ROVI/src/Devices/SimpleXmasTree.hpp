#ifndef __SIMPLE_XMAS_TREE_H__
#define __SIMPLE_XMAS_TREE_H__

#include <memory>
#include <chrono>

#include <ArduinoIostream.hpp>

#include <BaseClasses/RoviDevice.hpp>
#include <Components/Output/NeoPixelComponent.hpp>
#include <Components/Output/FastLedComponent.hpp>
#include <Common/LED/LEDEffectFactory.hpp>
#include <Common/LED/Effects/AllEffects.hpp>
#include <Common/LED/Color/KnownColors.hpp>

namespace Rovi {
    namespace Devices {
        template<uint8_t PIN, std::size_t NB_PIXEL>
        class SimpleXmasTree {
            public:
                SimpleXmasTree(const int colorCircleDelay = 500, const std::string& name = "xmastree")
                : m_leds{std::make_shared<Rovi::Components::FastLedComponent<PIN, NB_PIXEL>>(name)}
                , m_on{true}
                , m_brightness{255}
                , m_color{std::make_shared<HSVColor>(0.0f, 1.0f, 0.5f)}
                , m_effect{LEDEffectFactory::getEffect("color_static", m_leds.get())} 
                , m_colorCircleDelay{colorCircleDelay}
                , m_timePerEffect{15}
                , m_effects{}
                , m_nextEffectSelection{std::chrono::system_clock::now()}
                {
                    Serial << "Init efects" << endl;
                    m_effects.push_back(Rovi::LEDEffectFactory::getEffect("white_static", m_leds.get()));
                    m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_static", m_leds.get()));
                    m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_flow", m_leds.get()));
                    m_effects.push_back(Rovi::LEDEffectFactory::getEffect("rainbow", m_leds.get()));
                    m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_red", m_leds.get()));
                    m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_blue", m_leds.get()));
                    m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_green", m_leds.get()));
                    m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_circle_purple", m_leds.get()));

                    std::cout << "Init neopixel" << std::endl;
                    m_leds->init();
                    setOn(m_on);
                    setBrightness(m_brightness);
                    setColor(m_color);
                    // setEffect(m_effect);
                    // m_leds->setBrightness(255);
                    // m_leds->setColor(std::make_shared<Rovi::HSVColor>(128,0.0f,0.5f));                  
                    selectRandomEffect();
                }

                virtual void setupEffects() = 0;


                void update() {
                    m_leds->update();

                    auto now = std::chrono::system_clock::now();
                    if(now > m_nextEffectSelection) {
                        selectRandomEffect();
                    }
                }

                // TODO: Move to some LED base class
                // Attention: Rotary encoder in SimpleAbsoluteHue base class
                bool isOn() const {
                    return m_on;
                }

                void setOn(bool on) {
                    m_leds->setOn(on);
                    m_on = m_leds->isOn();
                }

                uint8_t brightness() const {
                    return m_brightness;
                }

                void setBrightness(uint8_t brightness) {
                    setOn(true);
                    m_leds->setBrightness(brightness);
                    // Not sure why this is required but otherwise the brightness is not set at all when changing
                    // from 0 to a small percentage
                    if(m_brightness == 0) {
                        m_leds->setBrightness(brightness);
                    }
                    m_brightness = m_leds->brightness();
                }

                std::shared_ptr<Color> color() {
                    m_color = m_leds->color();
                    return m_color;
                }

                void setColor(const std::shared_ptr<Color>& color) {
                    auto effect = LEDEffectFactory::getEffect("color_static", m_leds.get());
                    setEffect(effect);   

                    m_leds->setColor(color);
                    m_color = m_leds->color();
                }

                uint32_t hue() const {
                    return m_leds->hue();
                }

                void setHue(uint32_t hue) {
                    std::cout << "--- SimpleAbsoluteHue::setHue() to " << hue << std::endl;
                    m_leds->setHue(hue);
                    m_color = m_leds->color();
                }

                std::shared_ptr<LEDEffect> effect() const {
                    return m_effect;
                }

                // TBD: Adapt?
                void setEffect(const std::shared_ptr<LEDEffect>& effect) {
                    m_leds->setEffect(effect); 
                    m_effect = m_leds->effect();
                }

                void setEffect(int effect) {
                    m_leds->setEffect(effect); 
                    m_effect = m_leds->effect();   
                }

            protected:
                void selectRandomEffect() {
                    std::cout << "Random effect" << std::endl;

                    auto effectIndex = random(0, m_effects.size());
                    setEffect(m_effects[effectIndex]);

                    std::cout << "Effect " << (int) effectIndex << " selected" << std::endl;

                    auto now = std::chrono::system_clock::now();
                    m_nextEffectSelection = now + m_timePerEffect;
                }

                std::shared_ptr<Rovi::Components::FastLedComponent<PIN, NB_PIXEL>> m_leds;

                bool m_on;
                uint8_t m_brightness;
                std::shared_ptr<Color> m_color;
                std::shared_ptr<LEDEffect> m_effect;

                int m_colorCircleDelay;
                std::chrono::minutes m_timePerEffect;

                std::vector<std::shared_ptr<Rovi::LEDEffect>> m_effects;
                std::chrono::time_point<std::chrono::system_clock> m_nextEffectSelection;
        };
    };
};

#endif /* __SIMPLE_XMAS_TREE_H__ */