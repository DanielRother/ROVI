#ifndef __ROVI_MQTT_CONFIG_H__
#define __ROVI_MQTT_CONFIG_H__

#include <string>

namespace Rovi {
namespace Config {

struct MqttConfig {
  MqttConfig();
  MqttConfig(const char *server, const char *port, const char *user,
             const char *password);
  // TODO: Fix me (overload ambigous)
  // MqttConfig(const char *server, const int port, const char *user,
  //            const char *password);

  std::string toString();
  void print();

  std::string server;
  int port;
  std::string user;
  std::string password;

  static const int DEFAULT_PARAMETER_SIZE = 40;
};
} // namespace Config
} // namespace Rovi

#endif /* __ROVI_MQTT_CONFIG_H__ */