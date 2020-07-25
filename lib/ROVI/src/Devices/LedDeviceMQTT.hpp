#ifndef __LED_DEVICE_MQTT_H__
#define __LED_DEVICE_MQTT_H__

#include <Arduino.h>
#include <ArduinoJson.h>

#include "UtilFunctions.hpp"
#include "MqttDevice.hpp"
#include "SimpleLedDevice.hpp"

namespace Rovi {
    namespace Devices {
        template<class LED>
        class LedDeviceMQTT : public MqttDevice, public SimpleLedDevice<LED> {
            public:
                LedDeviceMQTT(Basecamp& iot, const std::shared_ptr<LED> led, const std::vector<std::shared_ptr<Rovi::LEDEffect>> effects, const std::string& name = "led")
                        : MqttDevice(iot)
                        , SimpleLedDevice<LED>(led, effects, name)
                        {
                            restoreSettings();
                            m_restoreStatus = RestoreStatus::RESTORED;
                            distributeSettings();
                        }

                virtual void setOn(bool on) override {
                    this->m_settingsChanged = true;
                    SimpleLedDevice<LED>::setOn(on);
                    this->distributeSettings();
                }

                virtual void setBrightness(uint8_t brightness) override {
                    this->m_settingsChanged = true;
                    SimpleLedDevice<LED>::setBrightness(brightness);
                    this->distributeSettings();
                }
                
                virtual void setColor(const std::shared_ptr<Color>& color) override {
                    m_settingsChanged = true;
                    SimpleLedDevice<LED>::setColor(color);
                    distributeSettings();
                }

                virtual void setHue(uint32_t hue) override {
                    m_settingsChanged = true;
                    SimpleLedDevice<LED>::setHue(hue);
                    distributeSettings();
                }

                virtual void setEffect(const std::shared_ptr<LEDEffect>& effect) override {
                    m_settingsChanged = true;
                    SimpleLedDevice<LED>::setEffect(effect);
                    distributeSettings();
                }

                virtual void setEffect(int effect) override {
                    m_settingsChanged = true;
                    SimpleLedDevice<LED>::setEffect(effect);
                    distributeSettings();
                }

                virtual void update() override {
                    SimpleLedDevice<LED>::update();
                    MqttDevice::update();
                }

            protected:
                virtual void saveSettings() override {
                    Serial << "LedDeviceMQTT::saveSettings() " << endl;

                    m_iot.configuration.set("rovi-power", String{this->m_on});
                    m_iot.configuration.set("rovi-brightness", String{this->m_brightness});
                    auto curRgbColor = this->color()->toRGB();
                    std::cout << "    save settings color: " << curRgbColor->toString() << std::endl;
                    m_iot.configuration.set("rovi-r", String{curRgbColor->r});
                    m_iot.configuration.set("rovi-g", String{curRgbColor->g});
                    m_iot.configuration.set("rovi-b", String{curRgbColor->b});
                    m_iot.configuration.set("rovi-effect", String{this->m_effect->name().c_str()});
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
                    // Check power last as power == false should always turn the bulb off
                    auto power = this->m_iot.configuration.get("rovi-power");
                    setOn(power.toInt());
                    std::cout << "****** Restored setting ******" << std::endl;
                    std::cout << "p = " << this->m_on << ", b = " << (uint32_t) this->m_brightness << "; c = " << color->toString() << "; e = " << this->m_effect->name() << std::endl;
                    std::cout << "******************************" << std::endl;
                }

                virtual void createMqttMessage(String& output) override {
                    Serial << "LedDeviceMQTT::createMqttMessage() " << endl;

                    const int capacity = 2000; //JSON_OBJECT_SIZE(10);
                    StaticJsonBuffer<capacity>jb;

                    // Create a JsonObject
                    JsonObject& obj = jb.createObject();

                    obj["power"] = this->m_on;
                    obj["brightness"] = this->m_brightness;
                    obj["colorType"] = "rgb";
                    auto curRgbColor = this->color()->toRGB();
                    std::cout << "    send color via mqttt: " << curRgbColor->toString() << std::endl;
                    JsonObject& color = jb.createObject();
                    color["r"] = curRgbColor->r;       
                    color["g"] = curRgbColor->g;       
                    color["b"] = curRgbColor->b;  
                    obj["color"] = color;     
                    obj["effect"] = String{this->m_effect->name().c_str()};

                    JsonObject& settings = jb.createObject();
                    JsonArray& possibleEffects =jb.createArray();
                    for(const auto effect : this->m_effects) {
                        possibleEffects.add(String{effect->name().c_str()});
                    }
                    settings["possibleEffects"] = possibleEffects;
                    obj["settings"] = settings;

                    obj.printTo(output);
                    std::cout << "msg: " << output << std::endl;
                }

                void receiveMqttMessage(const char* payload) override {
                    Serial << "LedDeviceMQTT::receiveMqttMessage() " << endl;

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
                            std::cout << "    r: " << r << ", g: " << g << ", b: " << b << std::endl;
                            std::cout << "    new color received via MQTT is " << newColor->toString();
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
                        auto selectedEffect = std::string{obj["effect"].as<char*>()};
                        std::cout << "Set effect to " << selectedEffect << endl;
                        for(auto effect : this->m_effects) {
                            if(effect->name() == selectedEffect) {
                                setEffect(effect);
                                break;
                            }
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

#endif /* __LED_DEVICE_MQTT_H__ */