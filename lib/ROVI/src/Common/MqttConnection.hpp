#ifndef __ROVI_MQTT_CONNECTION_H__
#define __ROVI_MQTT_CONNECTION_H__

#include <AsyncMqttClient.h>
#include <WiFi.h>

#include <iostream>
#include <list>

#include <enum.h>

#include <ArduinoIostream.hpp>
#include <Config/RoviWiFiManager.hpp>

namespace Rovi {
namespace Common {

BETTER_ENUM(MqttQoS, int, AT_MOST_ONCE, AT_LEAST_ONCE, EXCACTLY_ONCE);

class MqttConnection {
public:
  using OnConnectCallback = std::function<void(bool sessionPresent)>;
  using OnDisconnectCallback =
      std::function<void(AsyncMqttClientDisconnectReason reason)>;
  using OnSubscribeCallback =
      std::function<void(uint16_t packetId, uint8_t qos)>;
  using OnUnsubscribeCallback = std::function<void(uint16_t packetId)>;
  using OnMessageCallback = std::function<void(
      char *topic, char *payload, AsyncMqttClientMessageProperties properties,
      size_t len, size_t index, size_t total)>;
  using OnPublishCallback = std::function<void(uint16_t packetId)>;

  // TODO
  // enum class QoS {
  // }

  // MqttConnection(){};
  void start(Config::RoviWiFiManager &rwm) {
    addOnConnectCallback(
        std::bind(&MqttConnection::onMqttConnect, this, std::placeholders::_1));

    for (auto c : onConnectCalbacks) {
      mqtt.onConnect(c);
    }
    for (auto c : onDisconnectCallbacks) {
      mqtt.onDisconnect(c);
    }
    for (auto c : onSubscribeCallbacks) {
      mqtt.onSubscribe(c);
    }
    for (auto c : onUnsubscribeCallbacks) {
      mqtt.onUnsubscribe(c);
    }
    for (auto c : onMessageCallbacks) {
      mqtt.onMessage(c);
    }
    for (auto c : onPublishCallbacks) {
      mqtt.onPublish(c);
    }

    std::cout << "Set mqtt server config = " << rwm.mqttConfig.server.c_str()
              << ":" << rwm.mqttConfig.port << std::endl;
    std::cout << "Set mqtt server credentials = " << rwm.mqttConfig.user.c_str()
              << ", " << rwm.mqttConfig.password.c_str() << std::endl;

    strcpy(server, rwm.mqttConfig.server.c_str());
    port = rwm.mqttConfig.port;
    strcpy(user, rwm.mqttConfig.user.c_str());
    strcpy(password, rwm.mqttConfig.password.c_str());

    mqtt.setServer(server, port);
    mqtt.setCredentials(user, password);

    connect();
  }

  void addOnConnectCallback(OnConnectCallback callback) {
    onConnectCalbacks.push_back(callback);
  }
  void addOnDisconnectCallback(OnDisconnectCallback callback) {
    onDisconnectCallbacks.push_back(callback);
  }
  void addOnSubscribeCallback(OnSubscribeCallback callback) {
    onSubscribeCallbacks.push_back(callback);
  }
  void addOnUnsubscribeCallback(OnUnsubscribeCallback callback) {
    onUnsubscribeCallbacks.push_back(callback);
  }
  void addOnMessageCallback(OnMessageCallback callback) {
    onMessageCallbacks.push_back(callback);
  }
  void addOnPublishCallback(OnPublishCallback callback) {
    onPublishCallbacks.push_back(callback);
  }

  bool connected() const { return mqtt.connected(); }
  void connect() {
    std::cout << "Connecting to MQTT..." << std::endl;

    // // TODO: What is the sense behind these magics?
    // // If the MQTT client is not connected force a disconnect.
    // if (mqtt.connected() != 1) {
    //   mqttIsConnecting = false;
    //   mqtt.disconnect(true);
    // }
    // If the MQTT client is not connecting, not already connected and the WiFi
    // has a connection, try to connect
    if (!mqttIsConnecting) {
      if (mqtt.connected() != 1) {
        if (WiFi.status() == WL_CONNECTED) {
          mqtt.connect();
          mqttIsConnecting = true;
        } else {
          mqtt.disconnect();
        }
      }
    }
  }

  void disconnect(bool force = false) { mqtt.disconnect(force); }

  uint16_t subscribe(const std::string &topic,
                     MqttQoS qos = MqttQoS::AT_MOST_ONCE) {
    return mqtt.subscribe(topic.c_str(), qos);
  }
  uint16_t unsubscribe(const std::string &topic) {
    mqtt.unsubscribe(topic.c_str());
  }

  uint16_t publish(const std::string &topic, MqttQoS qos, bool retain,
                   const std::string &payload) {
    return mqtt.publish(topic.c_str(), qos, retain, payload.c_str(),
                        payload.size());
  }
  void setWill(const std::string &topic, MqttQoS qos, bool retain,
               const std::string &payload) {
    mqtt.setWill(topic.c_str(), qos, retain, payload.c_str(), payload.length());
  }

protected:
  void onMqttConnect(bool sessionPresent) { mqttIsConnecting = false; }

  AsyncMqttClient mqtt;

  std::list<OnConnectCallback> onConnectCalbacks;
  std::list<OnDisconnectCallback> onDisconnectCallbacks;
  std::list<OnSubscribeCallback> onSubscribeCallbacks;
  std::list<OnUnsubscribeCallback> onUnsubscribeCallbacks;
  std::list<OnMessageCallback> onMessageCallbacks;
  std::list<OnPublishCallback> onPublishCallbacks;

  bool mqttIsConnecting = false;

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
#endif /* __ROVI_MQTT_CONNECTION_H__ */
