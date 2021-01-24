#ifndef __ROVI_WIFI_MANAGER_H__
#define __ROVI_WIFI_MANAGER_H__

#include <string>

#include "MqttConfig.hpp"
#include "WiFiDefaultConfig.hpp"

namespace Rovi {
namespace Config {

class RoviWiFiManager {
public:
  RoviWiFiManager();

  // callback notifying us of the need to save config
  void saveConfigCallback();
  void setupSpiffs();

protected:
  // define your default values here, if there are different values in
  // config.json, they are overwritten.
  WiFiDefaultConfig wiFiDefaultConfig;
  MqttConfig mqttConfig;
  std::string otaPassword;

  // flag for saving data
  bool shouldSaveConfig;
};

} // namespace Config
} // namespace Rovi
#endif /* __ROVI_WIFI_MANAGER_H__ */