#ifndef __MQTT_DEVICE_H__
#define __MQTT_DEVICE_H__

#include <Arduino.h>
#include <string>
#include <iostream>

#include <Basecamp.hpp>

#include "ArduinoIostream.hpp"

#include "BasicDevice.hpp"

namespace Rovi {
    namespace Devices {
        class MqttDevice : public virtual BasicDevice {
            public:
                MqttDevice(Basecamp& iot)
                : BasicDevice(iot)
                , m_isConnected{false}
                , m_hostname{iot.hostname.c_str()}
                , m_statusTopic{"rovi/" + m_hostname + "/status"}
                , m_setTopic{"rovi/" + m_hostname + "/set"}
                , m_willTopic{"rovi/" + m_hostname + "/connection"}
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
                virtual void createMqttMessage(String& output) = 0;
                virtual void receiveMqttMessage(const char* payload) = 0;

                virtual void distributeSettings() {
                    std::cout << "MqttDevice::distributeSettings()" << std::endl;
                    BasicDevice::distributeSettings();

                    if(m_isConnected && m_restoreStatus == RestoreStatus::READY) {
                        auto output = String{""};
                        createMqttMessage(output);

                        m_iot.mqtt.publish(m_willTopic.c_str(), 1, true, "{\"status\":\"online\"}");
                        m_iot.mqtt.publish(m_statusTopic.c_str(), 1, true, output.c_str());
                    }
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

                bool m_isConnected;
                std::string m_hostname;
                std::string m_statusTopic;
                std::string m_setTopic;
                std::string m_willTopic;

                static const uint16_t IGNORE_MQTT_MSG_ON_STARTUP_TIME_MS;
        };

        const uint16_t MqttDevice::IGNORE_MQTT_MSG_ON_STARTUP_TIME_MS = 0;

    };
};

#endif /* __MQTT_DEVICE_H__ */