#ifndef __MQTT_DEVICE_H__
#define __MQTT_DEVICE_H__

#include <Arduino.h>
#include <iostream>
#include <string>

#include <Basecamp.hpp>

#include "ArduinoIostream.hpp"

#include "BasicDevice.hpp"

namespace Rovi {
namespace Devices {
class MqttDevice : public virtual BasicDevice {
public:
  MqttDevice(Basecamp &iot)
      : BasicDevice(iot), m_isConnected{false},
        m_hostname{iot.hostname.c_str()}, m_statusTopic{"rovi/" + m_hostname +
                                                        "/status"},
        m_setTopic{"rovi/" + m_hostname + "/set"}, m_willTopic{"rovi/" +
                                                               m_hostname +
                                                               "/connection"},
        m_lastMqttUpdateSend(0) {
    std::cout << "Set last will on topic: " << m_willTopic << std::endl;
    // m_iot.mqtt.setWill(m_willTopic.c_str(), 1, true,
    // "{\"status\":\"offline\"}");
    m_iot.mqtt.setWill(m_willTopic.c_str(), 1, true, "{\"online\":false}");

    m_iot.mqtt.onConnect(
        [&](bool sessionPresent) { mqttConnected(sessionPresent); });
    m_iot.mqtt.onSubscribe(
        [&](uint16_t packetId, uint8_t qos) { mqttSubscribed(packetId, qos); });
    m_iot.mqtt.onMessage([&](char *topic, char *payload,
                             AsyncMqttClientMessageProperties properties,
                             size_t len, size_t index, size_t total) {
      receiveSetMessage(topic, payload, properties, len, index, total);
    });
    m_iot.mqtt.onPublish([&](uint16_t packetId) { mqttPublished(packetId); });
  }

protected:
  virtual void createMqttMessage(String &output) = 0;
  virtual void receiveMqttMessage(const char *payload) = 0;

  virtual void update() {
    BasicDevice::update();

    if ((millis() - m_lastMqttUpdateSend) >
        NO_MQTT_MESSAGE_SEND_RESTART_TIMEOUT_MS) {
      //     std::cout << "No MQTT message send for at least " <<
      //     NO_MQTT_MESSAGE_SEND_RESTART_TIMEOUT_MS << " ms. Probably wifi
      //     connection is lost -> Restarting" << std::endl; auto
      //     wifiReconnnected = iot.wifi.reconnect(); // Not implemented?!
      //     //if(wifiReconnnected && !iot.mqtt.connected()) {
      //     //if(! && millis() > 15000) {
      //     //    iot.mqtt.connect();
      //     //}
      // } else if((millis() - m_lastMqttUpdateSend) > 2 *
      // NO_MQTT_MESSAGE_SEND_RESTART_TIMEOUT_MS) {
      ESP.restart();
    }
  }

  virtual void distributeSettings() {
    std::cout << "MqttDevice::distributeSettings()" << std::endl;
    BasicDevice::distributeSettings();

    if (m_isConnected && m_restoreStatus == RestoreStatus::READY) {
      auto output = String{""};
      createMqttMessage(output);

      // m_iot.mqtt.publish(m_willTopic.c_str(), 1, true,
      // "{\"status\":\"online\"}");
      m_iot.mqtt.publish(m_willTopic.c_str(), 1, true, "{\"online\":true}");
      m_iot.mqtt.publish(m_statusTopic.c_str(), 1, true, output.c_str());
    }
  }

  void mqttConnected(bool sessionPresent) {
    std::cout << "mqttConnected()" << std::endl;

    m_iot.mqtt.subscribe(m_setTopic.c_str(), 2);
    auto bootcounter = m_iot.preferences.getUInt("bootcounter", 0);
    m_iot.mqtt.publish(
        std::string{"rovi/" + m_hostname + "/bootcounter"}.c_str(), 1, true,
        String{bootcounter}.c_str());
    m_isConnected = true;
    distributeSettings();
  }

  void mqttSubscribed(uint16_t packetId, uint8_t qos) {
    std::cout << "mqttSubscribed()" << std::endl;
  }

  void mqttPublished(uint16_t packetId) {
    std::cout << "mqttPublished()" << std::endl;
    m_lastMqttUpdateSend = millis();
  }

  void receiveSetMessage(char *topic, char *payload,
                         AsyncMqttClientMessageProperties properties,
                         size_t len, size_t index, size_t total) {
    std::cout << "mqttMessage() -  topic = " << topic
              << ", payload = " << payload << std::endl;
    if (m_restoreStatus != RestoreStatus::READY &&
        millis() > IGNORE_MQTT_MSG_ON_STARTUP_TIME_MS) {
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

  unsigned long m_lastMqttUpdateSend;
  static const uint16_t IGNORE_MQTT_MSG_ON_STARTUP_TIME_MS;
  static const unsigned long NO_MQTT_MESSAGE_SEND_RESTART_TIMEOUT_MS;
};

const uint16_t MqttDevice::IGNORE_MQTT_MSG_ON_STARTUP_TIME_MS = 0;
const unsigned long MqttDevice::NO_MQTT_MESSAGE_SEND_RESTART_TIMEOUT_MS =
    5 * 60 * 1000;
}; // namespace Devices
}; // namespace Rovi

#endif /* __MQTT_DEVICE_H__ */