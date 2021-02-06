#include <Arduino.h>
#include <WiFi.h>

#include <ArduinoIostream.hpp>

#include <functional>
#include <iostream>
#include <string>

#include <Common/MqttConnection.hpp>
#include <Config/RoviWiFiManager.hpp>

class SomeMqttTestClass {
public:
  SomeMqttTestClass(Rovi::Common::MqttConnection &mqtt) : mqtt{mqtt} {};

  //   // void WiFiEvent(WiFiEvent_t event) {
  //   //   Serial.printf("[WiFi-event] event: %d\n", event);
  //   //   switch (event) {
  //   //   case SYSTEM_EVENT_STA_GOT_IP:
  //   //     Serial.println("WiFi connected");
  //   //     Serial.println("IP address: ");
  //   //     Serial.println(WiFi.localIP());
  //   //     connectToMqtt();
  //   //     break;
  //   //   case SYSTEM_EVENT_STA_DISCONNECTED:
  //   //     Serial.println("WiFi lost connection");
  //   //     //   xTimerStop(
  //   //     //       mqttReconnectTimer,
  //   //     //       0); // ensure we don't reconnect to MQTT while
  //   //     reconnecting to
  //   //     //       Wi-Fi
  //   //     //   xTimerStart(wifiReconnectTimer, 0);
  //   //     break;
  //   //   }
  //   // }

  void onMqttConnect(bool sessionPresent) {
    std::cout << "Connected to MQTT." << std::endl;
    std::cout << "Session present: " << sessionPresent << std::endl;
    uint16_t packetIdSub = mqtt.subscribe("test/lol", 2);
    std::cout << "Subscribing at QoS 2, packetId: " << packetIdSub << std::endl;
    mqtt.publish("test/lol", 0, true, "test 1");
    std::cout << "Publishing at QoS 0" << std::endl;
    uint16_t packetIdPub1 = mqtt.publish("test/lol", 1, true, "test 2");
    std::cout << "Publishing at QoS 1, packetId: " << packetIdPub1 << std::endl;
    uint16_t packetIdPub2 = mqtt.publish("test/lol", 2, true, "test 3");
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
    std::cout << "Publish received." << std::endl
              << "\ttopic: " << topic << std::endl
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

protected:
  Rovi::Common::MqttConnection &mqtt;
};

Rovi::Common::MqttConnection mqttConnection;
SomeMqttTestClass mqttTest(mqttConnection);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  auto rwm = Rovi::Config::RoviWiFiManager();

  mqttConnection.addOnConnectCallback(std::bind(
      &SomeMqttTestClass::onMqttConnect, mqttTest, std::placeholders::_1));
  mqttConnection.addOnDisconnectCallback(std::bind(
      &SomeMqttTestClass::onMqttDisconnect, mqttTest, std::placeholders::_1));
  mqttConnection.addOnSubscribeCallback(
      std::bind(&SomeMqttTestClass::onMqttSubscribe, mqttTest,
                std::placeholders::_1, std::placeholders::_2));
  mqttConnection.addOnUnsubscribeCallback(std::bind(
      &SomeMqttTestClass::onMqttUnsubscribe, mqttTest, std::placeholders::_1));
  mqttConnection.addOnMessageCallback(std::bind(
      &SomeMqttTestClass::onMqttMessage, mqttTest, std::placeholders::_1,
      std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
      std::placeholders::_5, std::placeholders::_6));
  mqttConnection.addOnPublishCallback(std::bind(
      &SomeMqttTestClass::onMqttPublish, mqttTest, std::placeholders::_1));

  mqttConnection.start(rwm);
}

auto start = millis();

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    std::cerr << "WiFi isn't connected anymore. Restart ESP" << std::endl;
    delay(2000);
    ESP.restart();
  } // put your main code here, to run repeatedly:
}
