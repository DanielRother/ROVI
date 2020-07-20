#ifndef __ABSOLUTE_HUE_MQTT_H__
#define __ABSOLUTE_HUE_MQTT_H__

#include <ArduinoJson.h>

#include "MqttDevice.hpp"
#include "SimpleAbsoluteHue.h"
#include "Components/Input/RotaryEncoderWithButtonMQTT.hpp"

namespace Rovi {
    namespace Devices {
        class AbsoluteHueMQTT : public MqttDevice, public SimpleAbsoluteHue{
            public:
                AbsoluteHueMQTT(Basecamp& iot,
                        const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton,
                        const uint8_t neoPixelPin, const uint16_t nbNeoPixelLEDs)
                        : MqttDevice(iot)
                        , SimpleAbsoluteHue(pinA, pinB, pinButton, neoPixelPin, nbNeoPixelLEDs)
                        {
                            rotary = std::make_shared<Components::RotaryEncoderWithButtonMQTT>(pinA, pinB, pinButton);

                            setupNormal();
                            setupClick();
                            setupDoubleClick();
                            setupHold();

                            restoreSettings();
                            m_restoreStatus = RestoreStatus::RESTORED;
                            distributeSettings();
                        }

                virtual void setOn(bool on) override {
                    m_settingsChanged = true;
                    SimpleAbsoluteHue::setOn(on);
                    distributeSettings();
                }

                virtual void setBrightness(uint8_t brightness) override {
                    m_settingsChanged = true;
                    SimpleAbsoluteHue::setBrightness(brightness);
                    distributeSettings();
                }
                
                virtual void setColor(const std::shared_ptr<Color>& color) override {
                    m_settingsChanged = true;
                    SimpleAbsoluteHue::setColor(color);
                    distributeSettings();
                }

                virtual void setHue(uint32_t hue) override {
                    m_settingsChanged = true;
                    SimpleAbsoluteHue::setHue(hue);
                    distributeSettings();
                }

                virtual void setEffect(const std::shared_ptr<LEDEffect>& effect) override {
                    m_settingsChanged = true;
                    SimpleAbsoluteHue::setEffect(effect);
                    distributeSettings();
                }

                virtual void setEffect(int effect) override {
                    m_settingsChanged = true;
                    SimpleAbsoluteHue::setEffect(effect);
                    distributeSettings();
                }

                virtual void update() override {
                    SimpleAbsoluteHue::update();
                    MqttDevice::update();
                }

            protected:
                virtual void saveSettings() override {
                        m_iot.configuration.set("rovi-power", String{m_on});
                        m_iot.configuration.set("rovi-brightness", String{m_brightness});
                        auto curRgbColor = color()->toRGB();
                        m_iot.configuration.set("rovi-r", String{curRgbColor->r});
                        m_iot.configuration.set("rovi-g", String{curRgbColor->g});
                        m_iot.configuration.set("rovi-b", String{curRgbColor->b});
                        m_iot.configuration.set("rovi-effect", String{m_effect->name().c_str()});
                }

                virtual void restoreSettings() override {
                    auto power = m_iot.configuration.get("rovi-power");
                    setOn(power.toInt());
                    auto brightness = m_iot.configuration.get("rovi-brightness");
                    setBrightness(brightness.toInt());
                    auto r = m_iot.configuration.get("rovi-r");
                    auto g = m_iot.configuration.get("rovi-g");
                    auto b = m_iot.configuration.get("rovi-b");
                    auto color = std::make_shared<RGBColor>(r.toInt(), g.toInt(), b.toInt());
                    setColor(color);
                    auto effect = m_iot.configuration.get("rovi-effect");
                    auto it = std::find(m_possibleEffects.begin(), m_possibleEffects.end(), effect.c_str());
                    if (it != m_possibleEffects.end()) {
                        int index = std::distance(m_possibleEffects.begin(), it);
                        setEffect(index);
                    }
                    std::cout << "****** Restored setting ******" << std::endl;
                    std::cout << "p = " << m_on << ", b = " << (uint32_t) m_brightness << "; c = " << color->toString() << "; e = " << m_effect->name() << std::endl;
                    std::cout << "******************************" << std::endl;
                }

                virtual void createMqttMessage(String& output) override {
                    const int capacity = 2000; //JSON_OBJECT_SIZE(10);
                    StaticJsonBuffer<capacity>jb;

                    // Create a JsonObject
                    JsonObject& obj = jb.createObject();

                    obj["power"] = m_on;
                    obj["brightness"] = m_brightness;
                    obj["colorType"] = "rgb";
                    auto curRgbColor = color()->toRGB();
                    JsonObject& color = jb.createObject();
                    color["r"] = curRgbColor->r;       
                    color["g"] = curRgbColor->g;       
                    color["b"] = curRgbColor->b;  
                    obj["color"] = color;     
                    obj["effect"] = String{m_effect->name().c_str()};

                    JsonObject& settings = jb.createObject();
                    JsonArray& possibleEffects =jb.createArray();
                    for(const auto& effects : m_possibleEffects) {
                        possibleEffects.add(String{effects.c_str()});
                    }
                    settings["possibleEffects"] = possibleEffects;
                    obj["settings"] = settings;

                    auto rotaryMqtt = std::static_pointer_cast<Components::RotaryEncoderWithButtonMQTT>(rotary);
                    auto rotaryJsonSize = rotaryMqtt->jsonSize();
                    auto rotaryJson = new char[rotaryJsonSize];
                    rotaryMqtt->status(rotaryJson, rotaryJsonSize);
                    obj["rotary_button"] = RawJson(rotaryJson);

                    obj.printTo(output);
                    std::cout << "msg: " << output << std::endl;
                }

                void receiveMqttMessage(const char* payload) override {
                    const int capacity = 2000; //JSON_OBJECT_SIZE(10);
                    StaticJsonBuffer<capacity>jb;
                    JsonObject& obj = jb.parseObject(payload);
                    if(obj.success()) {
                        std::cout << "parseObject() succeeded" << std::endl;
                    } else {
                        std::cout << "parseObject() failed" << std::endl;
                        return;
                    }

                    // Check possible inputs
                    if(obj.containsKey("brightness")) {
                        auto brightness = Utils::clamp(obj["brightness"].as<int>(), 0, 100);
                        std::cout << "Set brightness to " << brightness << endl;
                        setBrightness(brightness);
                    }
                    if(obj.containsKey("colorType") && obj.containsKey("color")) {
                        auto colorType = std::string{obj["colorType"].as<char*>()};
                        std::cout << "Set color for type " << colorType << std::endl;
                        if(colorType == "rgb") {
                            auto color = obj["color"];
                            auto r = Utils::clamp(color["r"].as<int>(), 0, 255);
                            auto g = Utils::clamp(color["g"].as<int>(), 0, 255);
                            auto b = Utils::clamp(color["b"].as<int>(), 0, 255);
                            auto newColor = std::make_shared<RGBColor>(r, g, b);
                            setColor(newColor);
                        } else if(colorType == "hsv") {
                            auto color = obj["color"];
                            auto h = Utils::clamp(color["h"].as<int>(), 0, 359);
                            auto s = Utils::clamp(color["s"].as<float>(), 0.0f, 1.0f);
                            auto v = Utils::clamp(color["v"].as<float>(), 0.0f, 1.0f);
                            auto newColor = std::make_shared<HSVColor>(h, s, v);
                            setColor(newColor);
                        } else {
                            std::cout << "Error: Unknown colortype" << std::endl;
                        }
                    }
                    if(obj.containsKey("effect")) {
                        auto effect = std::string{obj["effect"].as<char*>()};
                        std::cout << "Set effect to " << effect << endl;
                        auto it = std::find(m_possibleEffects.begin(), m_possibleEffects.end(), effect);
                        if (it == m_possibleEffects.end()) {
                            std::cout << "Not a valid effect" << std::endl;
                        } else {
                            int index = std::distance(m_possibleEffects.begin(), it);
                            setEffect(index);
                        }
                    }
                    // Check power last as power == false should always turn the bulb off
                    if(obj.containsKey("power")) {
                        auto power = obj["power"].as<bool>();
                        std::cout << "Set power to " << power << endl;
                        setOn(power);
                    }
                }
        };

    };
};

#endif /* __ABSOLUTE_HUE_MQTT_H__ */