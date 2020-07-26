#ifndef __ABSOLUTE_HUE_MQTT_H__
#define __ABSOLUTE_HUE_MQTT_H__

#include <Arduino.h>
#include <ArduinoJson.h>

#include <iostream>
#include <sstream>

#include "UtilFunctions.hpp"
#include "MqttDevice.hpp"
#include "SimpleAbsoluteHue.hpp"

namespace Rovi {
    namespace Devices {
        template<class ROTARY, class LED>
        class AbsoluteHueMQTT : public MqttDevice, public SimpleAbsoluteHue<ROTARY, LED> {
            public:
                AbsoluteHueMQTT(Basecamp& iot, const std::shared_ptr<ROTARY> rotary,
                    const std::shared_ptr<LED> led, const std::vector<std::shared_ptr<Rovi::LEDEffect>> effects, 
                    const std::string& name = "AbsoluteHueMQTT", std::chrono::minutes timePerEffect = std::chrono::minutes{})
                        : BasicDevice(iot)
                        , MqttDevice(iot)
                        , SimpleAbsoluteHue<ROTARY, LED>(iot, rotary, led, effects, name, timePerEffect)
                        {
                            restoreSettings();
                            m_restoreStatus = RestoreStatus::RESTORED;
                            distributeSettings();
                        }

                virtual void setOn(bool on) override {
                    SimpleAbsoluteHue<ROTARY, LED>::setOn(on);
                    this->distributeSettings();
                }

                virtual void setBrightness(uint8_t brightness) override {
                    SimpleAbsoluteHue<ROTARY, LED>::setBrightness(brightness);
                    this->distributeSettings();
                }
                
                virtual void setColor(const std::shared_ptr<Color>& color) override {
                    SimpleAbsoluteHue<ROTARY, LED>::setColor(color);
                    distributeSettings();
                }

                virtual void setHue(uint32_t hue) override {
                    SimpleAbsoluteHue<ROTARY, LED>::setHue(hue);
                    distributeSettings();
                }

                virtual void setEffect(const std::shared_ptr<LEDEffect>& effect) override {
                    SimpleAbsoluteHue<ROTARY, LED>::setEffect(effect);
                    distributeSettings();
                }

                virtual void setEffect(int effect) override {
                    SimpleAbsoluteHue<ROTARY, LED>::setEffect(effect);
                    distributeSettings();
                }

                virtual void setTimePerEffect(const std::chrono::minutes& timePerEffect) override {
                    SimpleAbsoluteHue<ROTARY, LED>::setTimePerEffect(timePerEffect);
                    distributeSettings();
                }
                
                virtual void update() override {
                    SimpleAbsoluteHue<ROTARY, LED>::update();
                    MqttDevice::update();
                }

            protected:
                // TODO/TBD: Reuse mqtt implementation of LedDeviceMQTT and only at rotary stuff here?
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
                    std::stringstream ss;
                    ss << this->m_timePerEffect.count();
                    obj["timePerEffect_m"] = String{ss.str().c_str()}.toInt();

                    JsonObject& settings = jb.createObject();
                    JsonArray& possibleEffects =jb.createArray();
                    for(const auto effect : this->m_effects) {
                        possibleEffects.add(String{effect->name().c_str()});
                    } 
                    settings["possibleEffects"] = possibleEffects;
                    obj["settings"] = settings;

                    auto rotaryMqtt = std::static_pointer_cast<Components::RotaryEncoderWithButtonMQTT>(this->m_rotary);
                    auto rotaryJsonSize = rotaryMqtt->jsonSize();
                    auto rotaryJson = new char[rotaryJsonSize];
                    rotaryMqtt->status(rotaryJson, rotaryJsonSize);
                    obj["rotary_button"] = RawJson(rotaryJson);                    

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
                    if(obj.containsKey("timePerEffect_m")) {
                        auto timePerEffect = obj["timePerEffect_m"].as<int>();
                        std::cout << "Set timePerEffect to " << timePerEffect << endl;
                        setTimePerEffect(std::chrono::minutes{timePerEffect});
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