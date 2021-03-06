#ifndef __BASIC_DEVICE_H__
#define __BASIC_DEVICE_H__

#include <Arduino.h>

#include <iostream>
#include <string>

// #include <Basecamp.hpp>
#include <Common/SPIFFSSettingsInterface.hpp>

#include "ArduinoIostream.hpp"

namespace Rovi {
namespace Devices {
class BasicDevice : public SPIFFSSettingsInterface {
public:
  BasicDevice(const std::string &name = "basicDevice")
      : SPIFFSSettingsInterface{}, m_name{name}, m_settingsChangedTimestamp{0} {
  }

  virtual std::string getType() { return this->m_name; }

  virtual void getOptions(JsonObject &options, DynamicJsonBuffer &jsonBuffer) {
    std::cout << "BasicDevice has got no options" << std::endl;
  }

  // TODO: Remove me
  virtual void saveSettingsImpl(JsonObject &settings,
                                DynamicJsonBuffer &buffer){};
  virtual void restoreSettingsImpl(JsonObject &settings){};

protected:
  virtual void update() {
    if (m_settingsChangedTimestamp != 0 &&
        millis() - m_settingsChangedTimestamp > SAVE_SETTINGS_THRESHOLD_MS) {
      saveSettings();
      m_settingsChangedTimestamp = 0;
      std::cout << "BasicDevice::update() - Settings saved to disc" << endl;
    }
  }

  std::string m_name;
  unsigned long m_settingsChangedTimestamp;
  static const unsigned long SAVE_SETTINGS_THRESHOLD_MS = 1000;
};

}; // namespace Devices
}; // namespace Rovi

#endif /* __BASIC_DEVICE_H__ */