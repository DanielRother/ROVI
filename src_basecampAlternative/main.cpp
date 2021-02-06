#include <Arduino.h>
#include <WiFi.h>

#include <ArduinoIostream.hpp>

#include <functional>
#include <iostream>
#include <string>

#include <Common/MqttConnection.hpp>
#include <Common/WifiEventHandler.hpp>
#include <Config/RoviWiFiManager.hpp>

class SomeMqttTestClass {
public:
  SomeMqttTestClass(Rovi::Common::MqttConnection &mqtt)
      : mqtt{mqtt}, lastUpdate_ms{millis()} {
    mqtt.setWill(
        TEST_TOPIC, Rovi::Common::MqttQoS::AT_LEAST_ONCE, true,
        WILL_MSG); // Attention: topic and will must be at least a member, as
                   // AsyncMqttClient only holds a pointer and hence these
                   // variable cannot go out of scope...

    mqtt.addOnConnectCallback(std::bind(&SomeMqttTestClass::onMqttConnect, this,
                                        std::placeholders::_1));
    mqtt.addOnDisconnectCallback(std::bind(&SomeMqttTestClass::onMqttDisconnect,
                                           this, std::placeholders::_1));
    mqtt.addOnSubscribeCallback(std::bind(&SomeMqttTestClass::onMqttSubscribe,
                                          this, std::placeholders::_1,
                                          std::placeholders::_2));
    mqtt.addOnUnsubscribeCallback(std::bind(
        &SomeMqttTestClass::onMqttUnsubscribe, this, std::placeholders::_1));
    mqtt.addOnMessageCallback(std::bind(
        &SomeMqttTestClass::onMqttMessage, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
        std::placeholders::_5, std::placeholders::_6));
    mqtt.addOnPublishCallback(std::bind(&SomeMqttTestClass::onMqttPublish, this,
                                        std::placeholders::_1));
  };

  void onMqttConnect(bool sessionPresent) {
    std::cout << "Connected to MQTT." << std::endl;
    std::cout << "Session present: " << sessionPresent << std::endl;
    uint16_t packetIdSub =
        mqtt.subscribe("test/lol", Rovi::Common::MqttQoS::EXCACTLY_ONCE);
    std::cout << "Subscribing at QoS 2, packetId: " << packetIdSub << std::endl;
    mqtt.publish("test/lol", Rovi::Common::MqttQoS::AT_MOST_ONCE, true,
                 "test 1");
    std::cout << "Publishing at QoS 0" << std::endl;
    uint16_t packetIdPub1 = mqtt.publish(
        "test/lol", Rovi::Common::MqttQoS::AT_LEAST_ONCE, true, "test 2");
    std::cout << "Publishing at QoS 1, packetId: " << packetIdPub1 << std::endl;
    uint16_t packetIdPub2 = mqtt.publish(
        "test/lol", Rovi::Common::MqttQoS::EXCACTLY_ONCE, true, "test 3");
    std::cout << "Publishing at QoS 2, packetId: " << packetIdPub2 << std::endl;
  }

  void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    std::cout << "Disconnected from MQTT." << std::endl;

    if (WiFi.isConnected()) {
      //   xTimerStart(mqttReconnectTimer, 0);
    }
  }

  void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
    std::cout << "Subscribe acknowledged." << std::endl
              << "\tpacketId: " << packetId << std::endl
              << "\tqos: " << (int)qos << std::endl;
  }

  void onMqttUnsubscribe(uint16_t packetId) {
    std::cout << "Unsubscribe acknowledged." << std::endl
              << "\tpacketId: " << packetId << std::endl;
  }

  void onMqttMessage(char *topic, char *payload,
                     AsyncMqttClientMessageProperties properties, size_t len,
                     size_t index, size_t total) {
    std::cout << "Message received." << std::endl
              << "\ttopic: " << topic << std::endl
              << "\tpayload: " << payload << std::endl
              << "\tqos: " << (int)properties.qos << std::endl
              << "\tdup: " << properties.dup << std::endl
              << "\tretain: " << properties.retain << std::endl
              << "\tlen: " << len << std::endl
              << "\tindex: " << index << std::endl
              << "\ttotal: " << total << std::endl;
  }

  void onMqttPublish(uint16_t packetId) {
    std::cout << "Publish acknowledged." << std::endl;
    std::cout << "\tpacketId: " << packetId << std::endl;
  }

  void update() {
    auto now_ms = millis();
    if (now_ms - lastUpdate_ms > TIME_BETWEEN_UPDATES_MS) {
      if (!mqtt.connected()) {
        mqtt.connect();
      }

      std::cout << "Update -> Send MQTT message" << std::endl;
      auto msg = "{\"power\": true}";
      mqtt.publish("esp/online", Rovi::Common::MqttQoS::AT_LEAST_ONCE, true,
                   msg);

      lastUpdate_ms = now_ms;
    }
  }

protected:
  Rovi::Common::MqttConnection &mqtt;
  unsigned long lastUpdate_ms;
  static const unsigned long TIME_BETWEEN_UPDATES_MS = 5000;
  static const std::string TEST_TOPIC;
  static const std::string WILL_MSG;
};
const std::string SomeMqttTestClass::TEST_TOPIC = "esp/online";
const std::string SomeMqttTestClass::WILL_MSG = "{\"power\": false}";

Rovi::Common::MqttConnection mqttConnection;
SomeMqttTestClass mqttTestDevice(mqttConnection);

void setup() {
  Serial.begin(115200);
  WiFi.onEvent(wiFiEventHandler);

  auto rwm = Rovi::Config::RoviWiFiManager();
  mqttConnection.start(rwm);
}

auto start = millis();

void loop() { mqttTestDevice.update(); }
