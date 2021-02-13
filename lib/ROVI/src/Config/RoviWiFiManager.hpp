#ifndef __ROVI_WIFI_MANAGER_H__
#define __ROVI_WIFI_MANAGER_H__

#include <string>

#include "MqttConfig.hpp"
#include "WiFiConfig.hpp"

#include <Common/SPIFFSSettingsInterface.hpp>

namespace Rovi {
namespace Config {

class RoviWiFiManager : public SPIFFSSettingsInterface {
public:
  RoviWiFiManager();

  // Define your default values here, if there are different values in
  // config.json, they are overwritten.
  std::string deviceName;
  WiFiConfig wiFiConfig;
  MqttConfig mqttConfig;
  std::string otaPassword;

protected:
  // callback notifying us of the need to save config
  void saveConfigCallback();
  virtual void saveSettingsImpl(JsonObject &json, DynamicJsonBuffer &buffer);
  virtual void restoreSettingsImpl(JsonObject &settings);

  WiFiConfig wiFiDefaultConfig;
  // flag for saving data
  bool shouldSaveConfig;

  static const int DEFAULT_STRING_PARAMETER_SIZE = 40;
  static const int CONFIG_PORTAL_TIMEOUT_SECONDS = 180;
};

} // namespace Config
} // namespace Rovi
#endif /* __ROVI_WIFI_MANAGER_H__ */