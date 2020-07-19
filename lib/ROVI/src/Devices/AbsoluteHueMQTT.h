#ifndef __ABSOLUTE_HUE_MQTT_H__
#define __ABSOLUTE_HUE_MQTT_H__

#include <ArduinoJson.h>

#include "SimpleAbsoluteHue.h"
#include "Components/Input/RotaryEncoderWithButtonMQTT.hpp"

namespace Rovi {
    namespace Devices {
        class AbsoluteHueMQTT : public SimpleAbsoluteHue {
            public:
                AbsoluteHueMQTT(const uint8_t pinA, const uint8_t pinB, const uint8_t pinButton,
                        const uint8_t neoPixelPin, const uint16_t nbNeoPixelLEDs)
                        : SimpleAbsoluteHue(pinA, pinB, pinButton, neoPixelPin, nbNeoPixelLEDs)
                        , lastStateStatusSend_ms{0}
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
                bool isStatusUpdateRequired() {
                    return millis() - lastStateStatusSend_ms > STATE_STATUS_SEND_TIMEOUT_MS;
                }

                void sendStateStatusUpdate() {
                    std::cout << "sendStateStatusUpdate()" << std::endl;

                    const int capacity = 2000; //JSON_OBJECT_SIZE(10);
                    StaticJsonBuffer<capacity>jb;

                    // Create a JsonObject
                    JsonObject& obj = jb.createObject();

                    auto rotaryMqtt = std::static_pointer_cast<Components::RotaryEncoderWithButtonMQTT>(rotary);
                    auto rotaryJsonSize = rotaryMqtt->jsonSize();
                    auto rotaryJson = new char[rotaryJsonSize];
                    rotaryMqtt->status(rotaryJson, rotaryJsonSize);
                    obj["rotary_button"] = RawJson(rotaryJson);
                    // std::cout << "rotary - size = " <<  rotaryJsonSize << " msg = " << rotaryJson << std::endl;

                    // std::static_pointer_cast<Components::RotaryEncoderWithButtonMQTT>(rotary)->status();
                    // obj["rotary_button"] = std::static_pointer_cast<Components::RotaryEncoderWithButtonMQTT>(rotary)->status();

                    JsonObject& obj2 = jb.createObject();
                    obj2["foo"] = 42;
                    obj2["bar"] = 48.748010;
                    obj["someNestedObj"] = obj2;

                    // Declare a buffer to hold the result
                    char output[1280];

                    // Produce a minified JSON document
                    obj.printTo(output);

                    // StaticJsonDocument<100> testDocument;       // TODO: Use Dynamic
                    // testDocument["sensorType"] = "Temperature";
                    // testDocument["value"] = 10;
                    // auto msg = std::string{};
                    // serializeJson(testDocument, msg);
                    std::cout << "msg: " << output << std::endl;

                    lastStateStatusSend_ms = millis();
                }

                unsigned long lastStateStatusSend_ms;

                static const uint16_t STATE_STATUS_SEND_TIMEOUT_MS;
        };

        const uint16_t AbsoluteHueMQTT::STATE_STATUS_SEND_TIMEOUT_MS = 5000;
    };
};

#endif /* __ABSOLUTE_HUE_MQTT_H__ */