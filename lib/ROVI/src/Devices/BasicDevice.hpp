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
  // State are required such that the first mqtt message can be ignoried
  enum class RestoreStatus { INIT, RESTORED, READY };

  BasicDevice()
      : SPIFFSSettingsInterface{}, m_restoreStatus{RestoreStatus::INIT},
        m_settingsChanged{false}, lastStateStatusSend_ms{0} {
    // m_iot.configuration.save();          // Bracuh ich das noch? War
    // vermutlich nur wegen WLAN
  }

protected:
  //   virtual void saveSettings() = 0;
  //   virtual void restoreSettings() = 0;

  virtual void update() {
    if (isStatusUpdateRequired()) {
      distributeSettings();
      m_restoreStatus =
          RestoreStatus::READY; // Ready for reception and normal processing

      if (m_settingsChanged) {
        saveSettings();
        m_settingsChanged = false;
        std::cout << "Settings saved to disc" << endl;
      }
    }
  }

  bool isStatusUpdateRequired() {
    return millis() - lastStateStatusSend_ms > STATE_STATUS_SEND_TIMEOUT_MS;
  }

  // TODO/TBD: Rename and break diamond inheritance?
  virtual void distributeSettings() {
    std::cout << "BasicDevice::distributeSettings()" << std::endl;

    if (m_restoreStatus == RestoreStatus::READY && m_settingsChanged) {
      //   saveSettings();        // Das war das alte Klassen saveSettings()
      std::cout << "Settings saved to RAM" << endl;
    }

    lastStateStatusSend_ms = millis();
  }

  RestoreStatus m_restoreStatus;

  bool m_settingsChanged;
  unsigned long lastStateStatusSend_ms;

  static const uint16_t STATE_STATUS_SEND_TIMEOUT_MS;
};

const uint16_t BasicDevice::STATE_STATUS_SEND_TIMEOUT_MS = 5000;
}; // namespace Devices
}; // namespace Rovi

#endif /* __BASIC_DEVICE_H__ */