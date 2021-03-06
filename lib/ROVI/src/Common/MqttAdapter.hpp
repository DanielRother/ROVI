#ifndef __MQTT_ADAPTER_H__
#define __MQTT_ADAPTER_H__

#include <Arduino.h>

#include <ArduinoIostream.hpp>
#include <Common/MqttConnection.hpp>
#include <Config/RoviWiFiManager.hpp>
#include <Devices/BasicDevice.hpp>

#include <iostream>
#include <string>

namespace Rovi {
namespace Common {
class MqttAdapter {
public:
  MqttAdapter(Devices::BasicDevice &device, Common::MqttConnection &mqtt,
              Config::RoviWiFiManager &rwm)
      : m_device{device}, m_mqtt{mqtt}, m_rwm{rwm}, m_isConnected{false},
        m_hostname{rwm.deviceName}, m_statusTopic{"rovi/" + m_hostname +
                                                  "/status"},
        m_setTopic{"rovi/" + m_hostname + "/set"}, m_willTopic{"rovi/" +
                                                               m_hostname +
                                                               "/connection"},
        m_infoTopic{"rovi/" + m_hostname + "/info"},
        m_lastStateStatusSend_ms{0}, m_lastMqttMsgPublished(0) {
    std::cout << "MQTT topics: " << std::endl
              << "m_hostname: " << m_hostname << std::endl
              << "m_statusTopic: " << m_statusTopic << std::endl
              << "m_setTopic: " << m_setTopic << std::endl
              << "m_willTopic: " << m_willTopic << std::endl;

    m_mqtt.setWill(m_willTopic, Common::MqttQoS::AT_LEAST_ONCE, true,
                   "{\"online\": false}");

    m_mqtt.addOnConnectCallback(
        std::bind(&MqttAdapter::mqttConnected, this, std::placeholders::_1));
    m_mqtt.addOnDisconnectCallback(
        std::bind(&MqttAdapter::mqttDisconnect, this, std::placeholders::_1));
    m_mqtt.addOnSubscribeCallback(std::bind(&MqttAdapter::mqttSubscribed, this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
    // TBD: Is there anything usefull for unsubscripe and is this required in
    // this class?
    // m_mqtt.addOnUnsubscribeCallback(
    //     std::bind(&MqttAdapter::onMqttUnsubscribe, this,
    //     std::placeholders::_1));
    m_mqtt.addOnMessageCallback(std::bind(
        &MqttAdapter::mqttMessageReceived, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
        std::placeholders::_5, std::placeholders::_6));
    m_mqtt.addOnPublishCallback(
        std::bind(&MqttAdapter::mqttPublished, this, std::placeholders::_1));
  }

  void update() {
    m_device.update();

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

protected:
  bool isStatusUpdateRequired() {
    return millis() - m_lastStateStatusSend_ms > STATE_STATUS_SEND_TIMEOUT_MS;
  }

  void distributeSettings() {
    std::cout << "MqttAdapter::distributeSettings()" << std::endl;
    m_lastStateStatusSend_ms = millis();

    if (m_isConnected) {
      auto output = createStatusMessage();

      m_mqtt.publish(m_willTopic, Common::MqttQoS::AT_LEAST_ONCE, true,
                     "{\"online\": true}");
      m_mqtt.publish(m_statusTopic, Common::MqttQoS::AT_LEAST_ONCE, true,
                     output.c_str());
    }
  }

  std::string createStatusMessage() {
    std::cout << "MqttAdapter::createMqttMessage() " << std::endl;

    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    m_device.settingsToJson(json, jsonBuffer);
    // Altough documented other, the library doesn't support std::string... :(
    // auto output = std::string{};
    // std::string output;
    String output;
    json.printTo(output);
    return output.c_str();
  }

  std::string createInfoMessage() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["ip"] = WiFi.localIP().toString();
    json["mac"] = WiFi.macAddress();
    json["type"] = m_device.getType().c_str();
    JsonObject &options = jsonBuffer.createObject();
    m_device.getOptions(options, jsonBuffer);
    json["options"] = options;

    String output;
    json.printTo(output);
    json.prettyPrintTo(Serial);
    return output.c_str();
  }

  void receiveSetMessage(const std::string &payload) {
    std::cout << "MqttAdapter::receiveMqttMessage() " << std::endl;

    DynamicJsonBuffer jsonBuffer;
    String convPayload = payload.c_str();
    JsonObject &json = jsonBuffer.parseObject(convPayload);
    json.prettyPrintTo(Serial);
    m_device.jsonToSettings(json);
    distributeSettings();
  }

  void mqttConnected(bool sessionPresent) {
    std::cout << "mqttConnected()" << std::endl;
    std::cout << "subscribe to topic: " << m_setTopic << std::endl;

    m_mqtt.subscribe(m_setTopic, Common::MqttQoS::EXCACTLY_ONCE);
    m_isConnected = true;
    distributeSettings();

    auto info = createInfoMessage();
    m_mqtt.publish(m_infoTopic, Common::MqttQoS::AT_LEAST_ONCE, true,
                   info.c_str());
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

  void mqttMessageReceived(char *topic, char *payload,
                           AsyncMqttClientMessageProperties properties,
                           size_t len, size_t index, size_t total) {
    std::cout << "mqttMessageReceived() -  topic = " << topic
              << ", payload = " << payload << std::endl;

    receiveSetMessage(payload);
  }

  Devices::BasicDevice &m_device;
  Common::MqttConnection &m_mqtt;
  Config::RoviWiFiManager &m_rwm;

  bool m_isConnected;
  std::string m_hostname;
  std::string m_statusTopic;
  std::string m_setTopic;
  std::string m_willTopic;
  std::string m_infoTopic;

  unsigned long m_lastStateStatusSend_ms;
  unsigned long m_lastMqttMsgPublished;

  static const uint16_t STATE_STATUS_SEND_TIMEOUT_MS = 5000;
  static const unsigned long NO_MQTT_MESSAGE_SEND_RESTART_TIMEOUT_MS =
      5 * 60 * 1000;
};
}; // namespace Common
}; // namespace Rovi

#endif /* __MQTT_ADAPTER_H__ */