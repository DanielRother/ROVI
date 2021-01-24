#ifndef __ROVI_WIFI_DEFAULT_CONFIG_H__
#define __ROVI_WIFI_DEFAULT_CONFIG_H__

#include <string>

namespace Rovi {
namespace Config {
struct WiFiDefaultConfig {
  WiFiDefaultConfig();

  std::string SSID;
  std::string password;
};
} // namespace Config
} // namespace Rovi

#endif /* __ROVI_WIFI_DEFAULT_CONFIG_H__ */