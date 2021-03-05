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
  BasicDevice() : SPIFFSSettingsInterface{}, m_settingsChanged{false} {}

protected:
  virtual void update() {
    if (m_settingsChanged) {
      saveSettings();
      m_settingsChanged = false;
      std::cout << "Settings saved to disc" << endl;
    }
  }

  bool m_settingsChanged;
};

}; // namespace Devices
}; // namespace Rovi

#endif /* __BASIC_DEVICE_H__ */