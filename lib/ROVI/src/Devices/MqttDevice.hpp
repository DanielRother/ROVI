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
                , m_settingsChanged{false}
                , lastStateStatusSend_ms{0}
                {
                    m_iot.web.addInterfaceElement("OTAPassword", "input", "OTA Password:", "#configform", "OTAPassword");
                    m_iot.web.setInterfaceElementAttribute("OTAPassword", "type", "password");
                    m_iot.configuration.save();

                    std::cout << "Set last will on topic: " << m_willTopic << std::endl;
                    m_iot.mqtt.setWill(m_willTopic.c_str(), 1, true, "{\"status\":\"offline\"}");

                    m_iot.mqtt.onConnect([&](bool sessionPresent) {mqttConnected(sessionPresent);});
                    m_iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {mqttSubscribed(packetId, qos);});
                    m_iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
                        size_t len, size_t index, size_t total) {receiveSetMessage(topic, payload, properties, len, index, total);});
                    m_iot.mqtt.onPublish([&](uint16_t packetId) {mqttPublished(packetId);});
                }

            protected:
                virtual void saveSettings() = 0;
                virtual void restoreSettings() = 0;
                virtual void createMqttMessage(String& output) = 0;
                virtual void receiveMqttMessage(const char* payload) = 0;

                void update() {
                    if(isStatusUpdateRequired()) {
                        distributeSettings();
                        m_restoreStatus = RestoreStatus::READY;     // Ready for reception and normal processing

                        if(m_settingsChanged) {
                            m_iot.configuration.save();
                            m_settingsChanged = false;
                            std::cout << "Settings saved to disc" << endl;
                        }
                    }
                }

                bool isStatusUpdateRequired() {
                    return millis() - lastStateStatusSend_ms > STATE_STATUS_SEND_TIMEOUT_MS;
                }

                void distributeSettings() {
                    std::cout << "distributeSettings()" << std::endl;

                    if(m_isConnected && m_restoreStatus == RestoreStatus::READY) {
                        auto output = String{""};
                        createMqttMessage(output);

                        m_iot.mqtt.publish(m_willTopic.c_str(), 1, true, "{\"status\":\"online\"}");
                        m_iot.mqtt.publish(m_statusTopic.c_str(), 1, true, output.c_str());
                    }

                    if(m_restoreStatus == RestoreStatus::READY && m_settingsChanged) {
                        saveSettings();
                        std::cout << "Settings saved to RAM" << endl;
                    }

                    lastStateStatusSend_ms = millis();
                }

                void mqttConnected(bool sessionPresent) {
                    std::cout << "mqttConnected()" << std::endl;

                    m_iot.mqtt.subscribe(m_setTopic.c_str(),2);
                    m_isConnected = true;
                    distributeSettings();
                }

                void mqttSubscribed(uint16_t packetId, uint8_t qos) {
                    std::cout << "mqttSubscribed()" << std::endl;
                }

                void mqttPublished(uint16_t packetId) {
                    std::cout << "mqttPublished()" << std::endl;
                }

                void receiveSetMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
                    std::cout << "mqttMessage() -  topic = " << topic << ", payload = " << payload << std::endl;
                    if(m_restoreStatus != RestoreStatus::READY && millis() > IGNORE_MQTT_MSG_ON_STARTUP_TIME_MS) {
                        std::cout << "Restoring not ready yet. Ignoring message" << std::endl;
                        return;
                    }

                    receiveMqttMessage(payload);
                }

                Basecamp& m_iot;
                bool m_isConnected;
                RestoreStatus m_restoreStatus;
                std::string m_hostname;
                std::string m_statusTopic;
                std::string m_setTopic;
                std::string m_willTopic;

                bool m_settingsChanged;
                unsigned long lastStateStatusSend_ms;

                static const uint16_t STATE_STATUS_SEND_TIMEOUT_MS;
                static const uint16_t IGNORE_MQTT_MSG_ON_STARTUP_TIME_MS;
        };

        const uint16_t MqttDevice::STATE_STATUS_SEND_TIMEOUT_MS = 5000;
        const uint16_t MqttDevice::IGNORE_MQTT_MSG_ON_STARTUP_TIME_MS = 0;

    };
};

#endif /* __MQTT_DEVICE_H__ */