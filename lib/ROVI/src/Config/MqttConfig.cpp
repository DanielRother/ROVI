#include "MqttConfig.hpp"

#include <iostream>
#include <sstream>
#include <stdlib.h> /* atoi */

namespace Rovi {
namespace Config {

MqttConfig::MqttConfig() : server{""}, port{1889}, user{""}, password{""} {
  server.reserve(DEFAULT_PARAMETER_SIZE);
  user.reserve(DEFAULT_PARAMETER_SIZE);
  password.reserve(DEFAULT_PARAMETER_SIZE);
}
MqttConfig::MqttConfig(const char *server, const char *port, const char *user,
                       const char *password)
    : server{server}, port{atoi(port)}, user{user}, password{password} {}
// TODO: Fix me (overload ambigous)
// MqttConfig::MqttConfig(const char *server, const int port, const char *user,
//            const char *password)
//     : server{server}, port{port}, user{user}, password{password} {}

std::string MqttConfig::toString() {
  std::stringstream ss;
  ss << "MqttConfig - server = " << server << ", port = " << port
     << ", user = " << user << ", password = " << password;
  return ss.str();
}

void MqttConfig::print() { std::cout << toString() << std::endl; }
} // namespace Config
} // namespace Rovi