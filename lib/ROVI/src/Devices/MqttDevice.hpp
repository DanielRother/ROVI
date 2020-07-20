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
                // State are required such that the first mqtt message can be ignoried
                enum class RestoreStatus {
                    INIT,
                    RESTORED,
                    READY
                };

                MqttDevice(Basecamp& iot)
                : m_iot(iot)
                , m_isConnected{false}
                , m_restoreStatus{RestoreStatus::INIT}
                , m_hostname{iot.hostname.c_str()}
                , m_statusTopic{"rovi/" + m_hostname + "/status"}
                , m_setTopic{"rovi/" + m_hostname + "/set"}
                , m_willTopic{"rovi/" + m_hostname + "/connection"}
                , lastStateStatusSend_ms{0}
                {
                    std::cout << "Set last will on topic: " << m_willTopic << std::endl;
                    m_iot.mqtt.setWill(m_willTopic.c_str(), 1, true, "{\"status\":\"offline\"}");

                    m_iot.mqtt.onConnect([&](bool sessionPresent) {mqttConnected(sessionPresent);});
                    m_iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {mqttSubscribed(packetId, qos);});
                    m_iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
                        size_t len, size_t index, size_t total) {receiveSetMessage(topic, payload, properties, len, index, total);});
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
                virtual void receiveSetMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) = 0;

                Basecamp& m_iot;
                bool m_isConnected;
                RestoreStatus m_restoreStatus;
                std::string m_hostname;
                std::string m_statusTopic;
                std::string m_setTopic;
                std::string m_willTopic;

                unsigned long lastStateStatusSend_ms;

                static const uint16_t STATE_STATUS_SEND_TIMEOUT_MS;
                static const uint16_t IGNORE_MQTT_MSG_ON_STARTUP_TIME_MS;
        };

        const uint16_t MqttDevice::STATE_STATUS_SEND_TIMEOUT_MS = 5000;
        const uint16_t MqttDevice::IGNORE_MQTT_MSG_ON_STARTUP_TIME_MS = 0;

    };
};

#endif /* __MQTT_DEVICE_H__ */