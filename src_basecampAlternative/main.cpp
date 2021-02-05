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
    Serial.println("Connected to MQTT.");
    Serial.print("Session present: ");
    Serial.println(sessionPresent);
    uint16_t packetIdSub = mqtt.subscribe("test/lol", 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);
    mqtt.publish("test/lol", 0, true, "test 1");
    Serial.println("Publishing at QoS 0");
    uint16_t packetIdPub1 = mqtt.publish("test/lol", 1, true, "test 2");
    Serial.print("Publishing at QoS 1, packetId: ");
    Serial.println(packetIdPub1);
    uint16_t packetIdPub2 = mqtt.publish("test/lol", 2, true, "test 3");
    Serial.print("Publishing at QoS 2, packetId: ");
    Serial.println(packetIdPub2);
  }

  void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.println("Disconnected from MQTT.");

    if (WiFi.isConnected()) {
      //   xTimerStart(mqttReconnectTimer, 0);
    }
  }

  void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
    Serial.println("Subscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
    Serial.print("  qos: ");
    Serial.println(qos);
  }

  void onMqttUnsubscribe(uint16_t packetId) {
    Serial.println("Unsubscribe acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
  }

  void onMqttMessage(char *topic, char *payload,
                     AsyncMqttClientMessageProperties properties, size_t len,
                     size_t index, size_t total) {
    Serial.println("Publish received.");
    Serial.print("  topic: ");
    Serial.println(topic);
    Serial.print("  qos: ");
    Serial.println(properties.qos);
    Serial.print("  dup: ");
    Serial.println(properties.dup);
    Serial.print("  retain: ");
    Serial.println(properties.retain);
    Serial.print("  len: ");
    Serial.println(len);
    Serial.print("  index: ");
    Serial.println(index);
    Serial.print("  total: ");
    Serial.println(total);
  }

  void onMqttPublish(uint16_t packetId) {
    Serial.println("Publish acknowledged.");
    Serial.print("  packetId: ");
    Serial.println(packetId);
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
