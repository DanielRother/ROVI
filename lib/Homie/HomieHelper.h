#ifndef __HOMIE_HELPER_H__
#define __HOMIE_HELPER_H__

#include <string>
#include <stdint.h>

namespace Rovi {
    namespace Homie{
        class TopicID {
            public:
                TopicID(const std::string& id);

                std::string id() const {return m_id; }
                std::string toString() const {return m_id; }

            protected:
                bool isValid(const std::string id) const;

                std::string m_id;
        };



        class Version {
            public:
                Version(const uint8_t major, const uint8_t minor, const uint8_t revision);

                uint8_t major() const { return m_major; };
                uint8_t minor() const { return m_minor; };
                uint8_t revision() const { return m_revision; };
                std::string toString() const;

            protected:
                uint8_t m_major;
                uint8_t m_minor;
                uint8_t m_revision;
        };
        


        // TODO: Move to some MQTT and/or ESP32 interface
        class HWInfo {
            public:
                HWInfo(const std::string& mac, const std::string& ip, const std::string& implementation);

                std::string mac() const { return m_mac; }
                std::string ip() const { return m_ip; }
                std::string implementation() const { return m_implementation; }
                std::string toString() const;

                // TODO: Implement (hardware specific)
                // Signal strength in %
                uint32_t signalStrength() const { return 100; }
                // CPU Temperature in °C
                uint32_t cpuTemperature() const { return 50; }
                // CPU Load in %. Average of last $interval including all CPUs 
                uint32_t cpuLoad() const { return 0; }
                // Battery level in %
                uint32_t batteryLevel() const { return 100; }
                // Free heap in bytes
                uint32_t freeheap() const { return 5*1024*1024; }
                // Supply Voltage in V
                float supplyVoltage() const { return 3.3f; }

            protected:
                std::string m_mac;      // TODO: Type=A1:B2:C3:D4:E5:F6
                std::string m_ip;
                std::string m_implementation;
        };
    }
}

#endif /* __HOMIE_HELPER_H__ */