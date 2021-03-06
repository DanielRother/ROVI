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
  BasicDevice() : SPIFFSSettingsInterface{}, m_settingsChangedTimestamp{0} {}

protected:
  virtual void update() {
    if (m_settingsChangedTimestamp != 0 &&
        millis() - m_settingsChangedTimestamp > SAVE_SETTINGS_THRESHOLD_MS) {
      saveSettings();
      m_settingsChangedTimestamp = 0;
      std::cout << "BasicDevice::update() - Settings saved to disc" << endl;
    }
  }

  unsigned long m_settingsChangedTimestamp;
  static const unsigned long SAVE_SETTINGS_THRESHOLD_MS = 1000;
};

}; // namespace Devices
}; // namespace Rovi

#endif /* __BASIC_DEVICE_H__ */