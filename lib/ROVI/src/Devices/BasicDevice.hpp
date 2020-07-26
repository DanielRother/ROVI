#ifndef __BASIC_DEVICE_H__
#define __BASIC_DEVICE_H__

#include <Arduino.h>
#include <string>
#include <iostream>

#include <Basecamp.hpp>

#include "ArduinoIostream.hpp"

namespace Rovi {
    namespace Devices {
        class BasicDevice {
            public:
                // State are required such that the first mqtt message can be ignoried
                enum class RestoreStatus {
                    INIT,
                    RESTORED,
                    READY
                };

                BasicDevice(Basecamp& iot)
                : m_iot(iot)
                , m_restoreStatus{RestoreStatus::INIT}
                , m_settingsChanged{false}
                , lastStateStatusSend_ms{0}
                {
                    m_iot.web.addInterfaceElement("OTAPassword", "input", "OTA Password:", "#configform", "OTAPassword");
                    m_iot.web.setInterfaceElementAttribute("OTAPassword", "type", "password");
                    m_iot.configuration.save();
                }

            protected:
                virtual void saveSettings() = 0;
                virtual void restoreSettings() = 0;

                virtual void update() {
                    if(isStatusUpdateRequired()) {
                        distributeSettings();
                        m_restoreStatus = RestoreStatus::READY;     // Ready for reception and normal processing

                        if(m_settingsChanged) {
                            m_iot.configuration.save();
                            m_settingsChanged = false;
                            std::cout << "Settings saved to disc" << endl;
                        }
                    }
                }

                bool isStatusUpdateRequired() {
                    return millis() - lastStateStatusSend_ms > STATE_STATUS_SEND_TIMEOUT_MS;
                }

                virtual void distributeSettings() {
                    std::cout << "BasicDevice::distributeSettings()" << std::endl;

                    if(m_restoreStatus == RestoreStatus::READY && m_settingsChanged) {
                        saveSettings();
                        std::cout << "Settings saved to RAM" << endl;
                    }

                    lastStateStatusSend_ms = millis();
                }

                Basecamp& m_iot;
                RestoreStatus m_restoreStatus;

                bool m_settingsChanged;
                unsigned long lastStateStatusSend_ms;

                static const uint16_t STATE_STATUS_SEND_TIMEOUT_MS;
        };

        const uint16_t BasicDevice::STATE_STATUS_SEND_TIMEOUT_MS = 5000;
    };
};

#endif /* __BASIC_DEVICE_H__ */