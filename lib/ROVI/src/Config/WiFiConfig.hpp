#ifndef __ROVI_WIFI_CONFIG_H__
#define __ROVI_WIFI_CONFIG_H__

#include <string>

namespace Rovi {
namespace Config {
struct WiFiConfig {
  WiFiConfig();
  WiFiConfig(const std::string &ssid, const std::string &password);

  std::string SSID;
  std::string password;
};
} // namespace Config
} // namespace Rovi

#endif /* __ROVI_WIFI_CONFIG_H__ */