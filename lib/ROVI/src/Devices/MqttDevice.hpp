#ifndef __MQTT_DEVICE_H__
#define __MQTT_DEVICE_H__

#include <Arduino.h>
#include <string>
#include <iostream>

#include <Basecamp.hpp>

#include "ArduinoIostream.hpp"

namespace Rovi {
    namespace Devices {
        class MqttDevice {
            public:
                MqttDevice(Basecamp& iot)
                : m_iot(iot)
                , m_isConnected{false}
                , m_hostname{iot.hostname.c_str()}
                , m_statusTopic{"rovi/" + m_hostname + "/status"}
                , m_setTopic{"rovi/" + m_hostname + "/set"}
                , lastStateStatusSend_ms{0}
                {
                    m_iot.mqtt.setWill(m_statusTopic.c_str(), 1, true, "{\"status\":\"offline\"}");

                    m_iot.mqtt.onConnect([&](bool sessionPresent) {mqttConnected(sessionPresent);});
                    m_iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {mqttSubscribed(packetId, qos);});
                    m_iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
                        size_t len, size_t index, size_t total) {mqttMessage(topic, payload, properties, len, index, total);});
                    m_iot.mqtt.onPublish([&](uint16_t packetId) {mqttPublished(packetId);});
                }

            protected:
                virtual void sendStateStatusUpdate() = 0;

                bool isStatusUpdateRequired() {
                    return millis() - lastStateStatusSend_ms > STATE_STATUS_SEND_TIMEOUT_MS;
                }

                //****************************************//
                // MQTT interface methods
                //****************************************//
                // TODO move to baseclass
                void mqttConnected(bool sessionPresent) {
                    std::cout << "mqttConnected()" << std::endl;

                    m_iot.mqtt.subscribe(m_setTopic.c_str(),2);
                    m_isConnected = true;
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

                Basecamp& m_iot;
                bool m_isConnected;
                std::string m_hostname;
                std::string m_statusTopic;
                std::string m_setTopic;

                unsigned long lastStateStatusSend_ms;

                static const uint16_t STATE_STATUS_SEND_TIMEOUT_MS;
        };

        const uint16_t MqttDevice::STATE_STATUS_SEND_TIMEOUT_MS = 5000;

    };
};

#endif /* __MQTT_DEVICE_H__ */