#ifndef __ABSOLUTE_HUE_MQTT_H__
#define __ABSOLUTE_HUE_MQTT_H__

#include <ArduinoJson.h>

#include "SimpleAbsoluteHue.h"
#include "Components/Input/RotaryEncoderWithButtonMQTT.hpp"

namespace Rovi {
    namespace Devices {
        class AbsoluteHueMQTT : public SimpleAbsoluteHue {
            public:
                AbsoluteHueMQTT(Basecamp& iot,
                        const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton,
                        const uint8_t neoPixelPin, const uint16_t nbNeoPixelLEDs)
                        : SimpleAbsoluteHue(pinA, pinB, pinButton, neoPixelPin, nbNeoPixelLEDs)
                        , lastStateStatusSend_ms{0}
                        , m_isConnected{false}
                        , m_hostname{""}
                        , m_statusTopic{""}
                        , m_setTopic{""}
                        {
                            // TODO move to baseclass
                            m_hostname = std::string{iot.hostname.c_str()};
                            m_setTopic = std::string{"rovi/" + m_hostname + "/set"};
                            m_statusTopic = std::string{"rovi/" + m_hostname + "/status"};

                            // const int capacity = 100; //JSON_OBJECT_SIZE(1);
                            // StaticJsonBuffer<capacity>jb;
                            // JsonObject& obj = jb.createObject();
                            // obj["status"] = "offline";
                            // char output[capacity];
                            // auto len = obj.printTo(output);
                            // std::cout << "last will: " << output << std::endl;
                            iot.mqtt.setWill(m_statusTopic.c_str(), 1, true, "{\"status\":\"offline\"}");

                            iot.mqtt.onConnect([&](bool sessionPresent) {mqttConnected(sessionPresent);});
                            iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {mqttSubscribed(packetId, qos);});
                            iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
                                size_t len, size_t index, size_t total) {mqttMessage(topic, payload, properties, len, index, total);});
                            iot.mqtt.onPublish([&](uint16_t packetId) {mqttPublished(packetId);});

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
                bool isStatusUpdateRequired() {
                    return millis() - lastStateStatusSend_ms > STATE_STATUS_SEND_TIMEOUT_MS;
                }

                //****************************************//
                // MQTT interface methods
                //****************************************//
                // TODO move to baseclass
                void mqttConnected(bool sessionPresent) {
                    std::cout << "mqttConnected()" << std::endl;

                    iot.mqtt.subscribe(m_setTopic.c_str(),2);
                    m_isConnected = true;//sessionPresent;
                    sendStateStatusUpdate();
                }
                void mqttSubscribed(uint16_t packetId, uint8_t qos) {
                    std::cout << "mqttSubscribed()" << std::endl;
                }
                void mqttPublished(uint16_t packetId) {
                    std::cout << "mqttPublished()" << std::endl;
                }
                void mqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
                    std::cout << "mqttMessage()" << std::endl;
                }


                void sendStateStatusUpdate() {
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
                        iot.mqtt.publish(m_statusTopic.c_str(), 1, true, output);
                    }

                    lastStateStatusSend_ms = millis();
                }

                unsigned long lastStateStatusSend_ms;

                static const uint16_t STATE_STATUS_SEND_TIMEOUT_MS;

                bool m_isConnected;
                std::string m_hostname;
                std::string m_statusTopic;
                std::string m_setTopic;
        };

        const uint16_t AbsoluteHueMQTT::STATE_STATUS_SEND_TIMEOUT_MS = 5000;
    };
};

#endif /* __ABSOLUTE_HUE_MQTT_H__ */