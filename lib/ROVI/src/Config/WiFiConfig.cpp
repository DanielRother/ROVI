#include "WiFiConfig.hpp"

#include <Esp.h>

#include <stdio.h>

namespace Rovi {
namespace Config {
WiFiConfig::WiFiConfig() : SSID{""}, password{"hakunamatata"} {
  uint32_t id = 0;
  for (int i = 0; i < 17; i = i + 8) {
    id |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  const int ID_STRING_LENGTH = 40;
  char idStr[ID_STRING_LENGTH];
  sprintf(idStr, "RoviConnectAP-%08X\n", id);
  SSID = idStr;
}

WiFiConfig::WiFiConfig(const std::string &ssid, const std::string &password)
    : SSID{ssid}, password{password} {}

} // namespace Config
} // namespace Rovi