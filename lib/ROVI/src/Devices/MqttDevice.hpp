#ifndef __MQTT_DEVICE_H__
#define __MQTT_DEVICE_H__

#include <Arduino.h>

#include <ArduinoIostream.hpp>
#include <Common/MqttConnection.hpp>
#include <Config/RoviWiFiManager.hpp>

#include <iostream>
#include <string>

#include "BasicDevice.hpp"

namespace Rovi {
namespace Devices {
class MqttDevice {
public:
  MqttDevice(Common::MqttConnection &mqtt, Config::RoviWiFiManager &rwm)
      : m_mqtt{mqtt}, m_rwm{rwm}, m_isConnected{false},
        m_hostname{rwm.deviceName}, m_statusTopic{"rovi/" + m_hostname +
                                                  "/status"},
        m_setTopic{"rovi/" + m_hostname + "/set"}, m_willTopic{"rovi/" +
                                                               m_hostname +
                                                               "/connection"},
        m_lastStateStatusSend_ms{0}, m_lastMqttMsgPublished(0) {
    std::cout << "MQTT topics: " << std::endl
              << "m_hostname: " << m_hostname << std::endl
              << "m_statusTopic: " << m_statusTopic << std::endl
              << "m_setTopic: " << m_setTopic << std::endl
              << "m_willTopic: " << m_willTopic << std::endl;

    m_mqtt.setWill(m_willTopic, Common::MqttQoS::AT_LEAST_ONCE, true,
                   "{\"online\": false}");

    m_mqtt.addOnConnectCallback(
        std::bind(&MqttDevice::mqttConnected, this, std::placeholders::_1));
    m_mqtt.addOnDisconnectCallback(
        std::bind(&MqttDevice::mqttDisconnect, this, std::placeholders::_1));
    m_mqtt.addOnSubscribeCallback(std::bind(&MqttDevice::mqttSubscribed, this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
    // TBD: Is there anything usefull for unsubscripe and is this required in
    // this class?
    // m_mqtt.addOnUnsubscribeCallback(
    //     std::bind(&MqttDevice::onMqttUnsubscribe, this,
    //     std::placeholders::_1));
    m_mqtt.addOnMessageCallback(std::bind(
        &MqttDevice::receiveSetMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
        std::placeholders::_5, std::placeholders::_6));
    m_mqtt.addOnPublishCallback(
        std::bind(&MqttDevice::mqttPublished, this, std::placeholders::_1));
  }

protected:
  virtual std::string createMqttMessage() = 0;
  virtual void receiveMqttMessage(const std::string &payload) = 0;

  virtual void update() {
    if (isStatusUpdateRequired()) {
      distributeSettings();

      if ((millis() - m_lastMqttMsgPublished) >
          NO_MQTT_MESSAGE_SEND_RESTART_TIMEOUT_MS) {
        std::cout << "No MQTT message send for at least "
                  << NO_MQTT_MESSAGE_SEND_RESTART_TIMEOUT_MS
                  << " ms. Probably wifi connection is lost -> Restarting "
                  << std::endl;
        delay(2000);
        ESP.restart();
      }
    }
  }

  virtual void distributeSettings() {
    std::cout << "MqttDevice::distributeSettings()" << std::endl;
    m_lastStateStatusSend_ms = millis();

    if (m_isConnected) {
      auto output = createMqttMessage();

      m_mqtt.publish(m_willTopic, Common::MqttQoS::AT_LEAST_ONCE, true,
                     "{\"online\": true}");
      m_mqtt.publish(m_statusTopic, Common::MqttQoS::AT_LEAST_ONCE, true,
                     output.c_str());
    }
  }

  bool isStatusUpdateRequired() {
    return millis() - m_lastStateStatusSend_ms > STATE_STATUS_SEND_TIMEOUT_MS;
  }

  void mqttConnected(bool sessionPresent) {
    std::cout << "mqttConnected()" << std::endl;
    std::cout << "subscribe to topic: " << m_setTopic << std::endl;

    m_mqtt.subscribe(m_setTopic, Common::MqttQoS::EXCACTLY_ONCE);
    m_isConnected = true;
    distributeSettings();
  }

  void mqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    std::cout << "mqttDisconnect()" << std::endl;

    if (WiFi.isConnected()) {
      //   xTimerStart(mqttReconnectTimer, 0);
      std::cerr << "Device lost wifi connection --> Restarting" << std::endl;
      delay(2000);
      ESP.restart();
    }
  }

  void mqttSubscribed(uint16_t packetId, uint8_t qos) {
    std::cout << "mqttSubscribed()" << std::endl;
  }

  void mqttPublished(uint16_t packetId) {
    std::cout << "mqttPublished()" << std::endl;
    m_lastMqttMsgPublished = millis();
  }

  void receiveSetMessage(char *topic, char *payload,
                         AsyncMqttClientMessageProperties properties,
                         size_t len, size_t index, size_t total) {
    std::cout << "mqttMessage() -  topic = " << topic
              << ", payload = " << payload << std::endl;

    receiveMqttMessage(payload);
  }

  Common::MqttConnection &m_mqtt;
  Config::RoviWiFiManager &m_rwm;

  bool m_isConnected;
  std::string m_hostname;
  std::string m_statusTopic;
  std::string m_setTopic;
  std::string m_willTopic;

  unsigned long m_lastStateStatusSend_ms;
  unsigned long m_lastMqttMsgPublished;

  static const uint16_t STATE_STATUS_SEND_TIMEOUT_MS;
  static const unsigned long NO_MQTT_MESSAGE_SEND_RESTART_TIMEOUT_MS;
};

const uint16_t MqttDevice::STATE_STATUS_SEND_TIMEOUT_MS = 5000;
const unsigned long MqttDevice::NO_MQTT_MESSAGE_SEND_RESTART_TIMEOUT_MS =
    5 * 60 * 1000;
}; // namespace Devices
}; // namespace Rovi

#endif /* __MQTT_DEVICE_H__ */