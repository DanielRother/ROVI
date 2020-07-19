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

                            sendStateStatusUpdate();
                        }

                virtual void update() override {
                    SimpleAbsoluteHue::update();
                    if(isStatusUpdateRequired()) {
                        sendStateStatusUpdate();
                    }
                }

                virtual void setOn(bool on) override {
                    SimpleAbsoluteHue::setOn(on);
                    sendStateStatusUpdate();
                }

                virtual void setBrightness(uint8_t brightness) override {
                    SimpleAbsoluteHue::setBrightness(brightness);
                    sendStateStatusUpdate();
                }
                
                virtual void setColor(const std::shared_ptr<Color>& color) override {
                    SimpleAbsoluteHue::setColor(color);
                    sendStateStatusUpdate();
                }

                virtual void setHue(uint32_t hue) override {
                    SimpleAbsoluteHue::setHue(hue);
                    sendStateStatusUpdate();
                }

                virtual void setEffect(const std::shared_ptr<LEDEffect>& effect) override {
                    SimpleAbsoluteHue::setEffect(effect);
                    sendStateStatusUpdate();
                }

                virtual void setEffect(int effect) override {
                    SimpleAbsoluteHue::setEffect(effect);
                    sendStateStatusUpdate();
                }

            protected:
                void sendStateStatusUpdate() override {
                    std::cout << "sendStateStatusUpdate()" << std::endl;

                    const int capacity = 2000; //JSON_OBJECT_SIZE(10);
                    StaticJsonBuffer<capacity>jb;

                    // Create a JsonObject
                    JsonObject& obj = jb.createObject();

                    obj["status"] = "online";
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

                    char output[1280];
                    obj.printTo(output);
                    std::cout << "msg: " << output << std::endl;

                    if(m_isConnected) {
                        m_iot.mqtt.publish(m_statusTopic.c_str(), 1, true, output);
                    }

                    lastStateStatusSend_ms = millis();
                }
        };

    };
};

#endif /* __ABSOLUTE_HUE_MQTT_H__ */