#ifndef __MQTT_TEST_H__
#define __MQTT_TEST_H__

/*
This example uses FreeRTOS softwaretimers as there is no built-in Ticker library
*/

#include <WiFi.h>
extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#include <Config/RoviWiFiManager.hpp>

namespace Rovi {
namespace Common {
class MqttTest {
public:
  MqttTest(){};
  void start(Config::RoviWiFiManager &rwm) /*: rwm{rwm}*/ {
    Serial.println();
    Serial.println();

    // TODO: Register Wifi callback handler

    // typedef void * TimerHandle_t;
    // typedef void (*TimerCallbackFunction_t)( TimerHandle_t xTimer );
    // mqttReconnectTimer = xTimerCreate(
    //     "mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
    //     reinterpret_cast<TimerCallbackFunction_t>([&]() { connectToMqtt();
    //     }));
    // wifiReconnectTimer =
    //     xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
    //                  reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
    // wifiReconnectTimer = xTimerCreate(
    //     "wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0,
    //     reinterpret_cast<TimerCallbackFunction_t>(&MqttTest::connectToWifi));

    // typedef void (*WiFiEventCb)(system_event_id_t event);
    // // #define WiFiEvent_t  system_event_id_t
    // WiFi.onEvent(&MqttTest::WiFiEvent);
    // auto foo = [&](system_event_id_t event) { WiFiEvent(event); };
    // WiFi.onEvent(reinterpret_cast<WiFiEventCb>(foo));

    mqttClient.onConnect(
        [&](bool sessionPresent) { onMqttConnect(sessionPresent); });
    mqttClient.onDisconnect([&](AsyncMqttClientDisconnectReason reason) {
      onMqttDisconnect(reason);
    });
    mqttClient.onSubscribe([&](uint16_t packetId, uint8_t qos) {
      onMqttSubscribe(packetId, qos);
    });
    mqttClient.onUnsubscribe(
        [&](uint16_t packetId) { onMqttUnsubscribe(packetId); });
    mqttClient.onMessage([&](char *topic, char *payload,
                             AsyncMqttClientMessageProperties properties,
                             size_t len, size_t index, size_t total) {
      onMqttMessage(topic, payload, properties, len, index, total);
    });
    mqttClient.onPublish([&](uint16_t packetId) { onMqttPublish(packetId); });

    Serial.println("Set mqtt server config:");
    Serial.println(rwm.mqttConfig.server.c_str());
    Serial.println(rwm.mqttConfig.port);
    Serial.println("Set mqtt server credentials:");
    Serial.println(rwm.mqttConfig.user.c_str());
    Serial.println(rwm.mqttConfig.password.c_str());

    strcpy(server, rwm.mqttConfig.server.c_str());
    port = rwm.mqttConfig.port;
    strcpy(user, rwm.mqttConfig.user.c_str());
    strcpy(password, rwm.mqttConfig.password.c_str());

    mqttClient.setServer(server, port);
    mqttClient.setCredentials(user, password);

    connectToWifi();
    connectToMqtt();
  }

  void connectToWifi() {
    Serial.println("WiFi should already be connected by WiFiManager -> Skip");
    return;

    // Serial.println("Connecting to Wi-Fi...");
    // WiFi.begin(rwm.wiFiConfig.SSID.c_str(),
    //            rwm.wiFiConfig.password.c_str());
  }

  void connectToMqtt() {
    Serial.println("Connecting to MQTT...");

    // From Bascamp
    bool mqttIsConnecting = false;

    // TODO: What is the sense behind these magics?
    // If the MQTT client is not connected force a disconnect.
    if (mqttClient.connected() != 1) {
      mqttIsConnecting = false;
      mqttClient.disconnect(true);
    }
    // If the MQTT client is not connecting, not already connected and the WiFi
    // has a connection, try to connect
    if (!mqttIsConnecting) {
      if (mqttClient.connected() != 1) {
        if (WiFi.status() == WL_CONNECTED) {
          mqttClient.connect();
          mqttIsConnecting = true;
        } else {
          mqttClient.disconnect();
        }
      }
    }

    // Original
    // mqttClient.connect();
  }

  void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      //   xTimerStop(
      //       mqttReconnectTimer,
      //       0); // ensure we don't reconnect to MQTT while reconnecting to
      //       Wi-Fi
      //   xTimerStart(wifiReconnectTimer, 0);
      break;
    }
  }

  void onMqttConnect(bool sessionPresent) {
    Serial.println("Connected to MQTT.");
    Serial.print("Session present: ");
    Serial.println(sessionPresent);
    uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
    Serial.print("Subscribing at QoS 2, packetId: ");
    Serial.println(packetIdSub);
    mqttClient.publish("test/lol", 0, true, "test 1");
    Serial.println("Publishing at QoS 0");
    uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
    Serial.print("Publishing at QoS 1, packetId: ");
    Serial.println(packetIdPub1);
    uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
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
  //   Config::RoviWiFiManager &rwm;

  AsyncMqttClient mqttClient;
  //   TimerHandle_t mqttReconnectTimer;
  //   TimerHandle_t wifiReconnectTimer;

  // Not sure why but the stupid old cstrings require a local copy. Otherwise
  // something seems to go out of scope or the pointer are for some other
  // fucking reason invalid.
  static const int maxCStringLength = 40;
  char server[maxCStringLength];
  int port;
  char user[maxCStringLength];
  char password[maxCStringLength];
};
} // namespace Common
} // namespace Rovi
#endif /* __MQTT_TEST_H__ */
