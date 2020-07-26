#ifndef __SIMPLE_LED_DEVICE_H__
#define __SIMPLE_LED_DEVICE_H__

#include <memory>
#include <chrono>
#include <iostream>

#include <Common/LED/LEDEffectFactory.hpp>
#include <Common/LED/Effects/AllEffects.hpp>
#include <Common/LED/Color/KnownColors.hpp>
#include "BasicDevice.hpp"

namespace Rovi {
    namespace Devices {
        template<class LED>
        class SimpleLedDevice : virtual BasicDevice {
            public:
                SimpleLedDevice(Basecamp& iot, const std::shared_ptr<LED> led, const std::vector<std::shared_ptr<Rovi::LEDEffect>> effects, const std::string& name = "led", std::chrono::minutes timePerEffect = std::chrono::minutes{15})
                : BasicDevice(iot)
                , m_name{name}
                , m_leds{led}
                , m_on{true}
                , m_brightness{128}
                , m_color{std::make_shared<HSVColor>(0.0f, 1.0f, 0.5f)}
                , m_effect{LEDEffectFactory::getEffect("color_static", m_leds.get())} 
                , m_effects{effects}
                , m_timePerEffect{timePerEffect}
                , m_nextEffectSelection{std::chrono::system_clock::now()}
                {
                    std::cout << "Init led" << std::endl;
                    m_leds->init();
                    restoreSettings();
                }

                virtual void update() {
                    m_leds->update();

                    auto now = std::chrono::system_clock::now();
                    if(now > m_nextEffectSelection && m_timePerEffect != std::chrono::minutes(0) && m_on) {
                        selectRandomEffect();
                    }

                    BasicDevice::update();
                }

                virtual bool isOn() const {
                    return m_on;
                }

                virtual void setOn(bool on) {
                    std::cout << "SimpleLedDevice::setOn(" << on << ")" << std::endl;
                    m_settingsChanged = true;
                    m_leds->setOn(on);
                    m_on = m_leds->isOn();
                }

                virtual uint8_t brightness() const {
                    return m_brightness;
                }

                virtual void setBrightness(uint8_t brightness) {
                    std::cout << "SimpleLedDevice::setBrightness(" << (int) brightness << ")" << std::endl;
                    m_settingsChanged = true;
                    setOn(true);
                    m_leds->setBrightness(brightness);
                    // Not sure why this is required but otherwise the brightness is not set at all when changing
                    // from 0 to a small percentage
                    if(m_brightness >= 0) {
                        m_leds->setBrightness(brightness);
                    }
                    m_brightness = m_leds->brightness();
                }

                virtual std::shared_ptr<Color> color() {
                    m_color = m_leds->color();
                    return m_color;
                }

                virtual void setColor(const std::shared_ptr<Color>& color) {
                    std::cout << "SimpleLedDevice::setColor(" << color << ")" << std::endl;
                    m_settingsChanged = true;
                    setOn(true);

                    auto selectedEffect = std::string{"color_static"};
                    bool effectFound = false;
                    for(auto effect : this->m_effects) {
                        if(effect->name() == selectedEffect) {
                            effectFound = true;
                            setEffect(effect);
                            break;
                        }
                    }  
                    if(!effectFound) {
                        auto effect = LEDEffectFactory::getEffect(selectedEffect, m_leds.get());
                        setEffect(effect); 
                        // Delay processing, otherwise the effect will update to really set color to the effect's default one...
                        sleep(1);
                        effect->update();
                    }

                    std::cout << "    finally realy set color" << std::endl;
                    m_leds->setColor(color);
                    m_color = m_leds->color();
                }

                virtual uint32_t hue() const {
                    return m_leds->hue();
                }

                virtual void setHue(uint32_t hue) {
                    std::cout << "SimpleLedDevice::setHue(" << hue << ")" << std::endl;
                    m_settingsChanged = true;
                    setOn(true);
                    m_leds->setHue(hue);
                    m_color = m_leds->color();
                }

                virtual std::shared_ptr<LEDEffect> effect() const {
                    return m_effect;
                }

                // TBD: Adapt?
                virtual void setEffect(const std::shared_ptr<LEDEffect>& effect) {
                    m_settingsChanged = true;
                    setOn(true);
                    m_leds->setEffect(effect); 
                    m_effect = m_leds->effect();
                }

                virtual void setEffect(int effect) {
                    m_settingsChanged = true;
                    setOn(true);
                    m_leds->setEffect(effect); 
                    m_effect = m_leds->effect();   
                }

               virtual void setEffects(const std::vector<std::shared_ptr<LEDEffect>>& effects) {
                    m_settingsChanged = true;
                    setOn(true);
                    m_effects = effects;
                    selectRandomEffect();
                }

                virtual std::vector<std::shared_ptr<LEDEffect>> getEffects() const {
                    return m_effects;
                }

                virtual std::chrono::minutes timePerEffect() const {
                    return m_timePerEffect;
                }

                virtual void setTimePerEffect(const std::chrono::minutes& timePerEffect)  {
                    m_settingsChanged = true;
                    m_timePerEffect = timePerEffect;
                    m_nextEffectSelection = std::chrono::system_clock::now() + m_timePerEffect;
                }


            protected:
                virtual void selectRandomEffect() {
                    std::cout << "SimpleLedDevice::selectRandomEffect()" << std::endl;

                    setOn(true);
                    auto effectIndex = random(0, m_effects.size());
                    setEffect(m_effects[effectIndex]);

                    std::cout << "Effect " << (int) effectIndex << " selected" << std::endl;

                    auto now = std::chrono::system_clock::now();
                    m_nextEffectSelection = now + m_timePerEffect;
                }

                virtual void saveSettings() override {
                    Serial << "LedDeviceMQTT::saveSettings() " << endl;

                    m_iot.configuration.set("rovi-power", String{this->m_on});
                    std::cout << "    save settings power: " << this->m_on << std::endl;
                    m_iot.configuration.set("rovi-brightness", String{this->m_brightness});
                    auto curRgbColor = this->color()->toRGB();
                    std::cout << "    save settings color: " << curRgbColor->toString() << std::endl;
                    m_iot.configuration.set("rovi-r", String{curRgbColor->r});
                    m_iot.configuration.set("rovi-g", String{curRgbColor->g});
                    m_iot.configuration.set("rovi-b", String{curRgbColor->b});
                    m_iot.configuration.set("rovi-effect", String{this->m_effect->name().c_str()});
                    std::cout << "    save settings effect: " << this->m_effect->name() << std::endl;
                    std::stringstream ss;
                    ss << this->m_timePerEffect.count();
                    m_iot.configuration.set("rovi-timePerEffect_m", String{ss.str().c_str()});
                }  

                virtual void restoreSettings() override {
                    Serial << "LedDeviceMQTT::restoreSettings() " << endl;

                    auto brightness = this->m_iot.configuration.get("rovi-brightness");
                    setBrightness(brightness.toInt());
                    auto r = this->m_iot.configuration.get("rovi-r");
                    auto g = this->m_iot.configuration.get("rovi-g");
                    auto b = this->m_iot.configuration.get("rovi-b");
                    auto color = std::make_shared<RGBColor>(r.toInt(), g.toInt(), b.toInt());
                    setColor(color);
                    auto selectedEffect = std::string{this->m_iot.configuration.get("rovi-effect").c_str()};
                    for(auto effect : this->m_effects) {
                        if(effect->name() == selectedEffect) {
                            setEffect(effect);
                            break;
                        }
                    }
                    auto timePerEffect = this->m_iot.configuration.get("rovi-timePerEffect_m").toInt();
                    setTimePerEffect(std::chrono::minutes{timePerEffect});

                    // Check power last as power == false should always turn the bulb off
                    auto power = this->m_iot.configuration.get("rovi-power");
                    setOn(power.toInt());
                    std::cout << "****** Restored setting ******" << std::endl;
                    std::cout << "p = " << this->m_on << ", b = " << (uint32_t) this->m_brightness << "; c = " << color->toString() << "; e = " << this->m_effect->name() << std::endl;
                    std::cout << "******************************" << std::endl;
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