#ifndef __SIMPLE_LED_DEVICE_H__
#define __SIMPLE_LED_DEVICE_H__

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
        template<class LED>
        class SimpleLedDevice {
            public:
                SimpleLedDevice(const std::shared_ptr<LED> led, const std::string& name = "led")
                : m_name{name}
                , m_leds{led}
                , m_on{true}
                , m_brightness{128}
                , m_color{std::make_shared<HSVColor>(0.0f, 1.0f, 0.5f)}
                , m_effect{LEDEffectFactory::getEffect("color_static", m_leds.get())} 
                , m_effects{LEDEffectFactory::getEffect("color_static", m_leds.get())}
                , m_timePerEffect{15}
                , m_nextEffectSelection{std::chrono::system_clock::now()}
                {
                    std::cout << "Init led" << std::endl;
                    m_leds->init();
                    setOn(m_on);
                    setBrightness(m_brightness);
                    setColor(m_color);
                }

                virtual void setupEffects() {
                    Serial << "Init efects" << endl;
                    m_effects.clear();
                    // m_effects.push_back(Rovi::LEDEffectFactory::getEffect("white_static", m_leds.get()));
                    // m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_static", m_leds.get()));
                    m_effects.push_back(Rovi::LEDEffectFactory::getEffect("color_flow", m_leds.get()));

                    selectRandomEffect();
                }

                void update() {
                    m_leds->update();

                    auto now = std::chrono::system_clock::now();
                    if(now > m_nextEffectSelection) {
                        selectRandomEffect();
                    }
                }

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

                std::string m_name;
                std::shared_ptr<LED> m_leds;

                bool m_on;
                uint8_t m_brightness;
                std::shared_ptr<Color> m_color;
                std::shared_ptr<LEDEffect> m_effect;

                std::vector<std::shared_ptr<Rovi::LEDEffect>> m_effects;
                std::chrono::minutes m_timePerEffect;
                std::chrono::time_point<std::chrono::system_clock> m_nextEffectSelection;
        };
    };
};

#endif /* __SIMPLE_LED_DEVICE_H__ */