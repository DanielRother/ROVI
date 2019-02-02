#ifndef __HOMIE_DEVICE_H__
#define __HOMIE_DEVICE_H__

#include <Arduino.h>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>

#include <FileIOUtils.hpp>

namespace Rovi {
    namespace  Homie {
        class TopicID {
            public:
                TopicID(const std::string& id) {
                    if(isValid(id)) {
                        m_id = id;
                    } else {
                        // TODO?
                    }
                }

                std::string toString() const {return m_id; }

            protected:
                // A topic level ID MAY contain lowercase letters from a to z, numbers from 0 to 9 as well as the hyphen character (-).
                // A topic level ID MUST NOT start or end with a hyphen (-). The special character $ is used and reserved for Homie attributes. The underscore (_) is used and reserved for Homie node arrays.
                bool isValid(const std::string id) {
                    auto isValid = bool{};
                    isValid &= checkStringForAllowedCharacters(id, std::string("abcdefghijklmnopqrstuvwxyz-01234567890"));
                    if(id.size() > 0) {
                        isValid &= (id.front() != '-');
                        isValid &= (id.back() != '-');
                    }
                }

                std::string m_id;
        };

        class Version {
            public:
                Version(const uint8_t major, const uint8_t minor, const uint8_t revision) 
                    : m_major{major}, m_minor{minor}, m_revision{revision}
                {}

                std::string toString() {
                    return to_string(m_major) + "." + to_string(m_minor) + "." + to_string(m_revision);
                }
            protected:
                uint8_t m_major;
                uint8_t m_minor;
                uint8_t m_revision;
        };

        // TODO: Move to some MQTT and/or ESP32 interface
        class HWInfo {
            public:
                HWInfo(const std::string& mac, const std::string& ip, const std::string& implementation)
                    : m_mac{mac}, m_ip{ip}, m_implementation{implementation}
                {} 

                std::string mac() const { return m_mac; }
                std::string ip() const { return m_ip; }
                std::string implementation() const { return m_implementation; }

            protected:
                std::string m_mac;      // TODO: Type=A1:B2:C3:D4:E5:F6
                std::string m_ip;
                std::string m_implementation;
        };

        class Device {
            public:
                using TopicType = std::string;      // TBD: MQTT topic path as list?
                using ValueType = std::string;
                using AttributeType = std::pair<TopicType, ValueType>;

                Device(const std::string deviceName, const HWInfo& hwInfo, 
                    const std::string& firmwareName, const Version& firmwareVersion,
                    const uint8_t statsInterval_s)
                    : m_deviceID{nameToTopic(deviceName) + "-" + macToTopic(hwInfo)}, 
                      m_homie{3, 0, 1}, m_name{deviceName}, 
                      m_localip{hwInfo.ip()}, m_mac{hwInfo.mac()},
                      m_fw_name{firmwareName}, m_fw_version{firmwareVersion}, 
                      m_implementation{hwInfo.implementation()}, m_statsInterval_s{statsInterval_s}
                    {}

            std::vector<AttributeType> refresh() {
                // TODO: Wo wird das Intervall gecheckt?        default = 60
                auto deviceAttributes = std::vector<AttributeType>{};
                deviceAttributes.emplace_back(deviceAttribute("$homie", m_homie.toString()));
                deviceAttributes.emplace_back(deviceAttribute("$name", m_name));
                deviceAttributes.emplace_back(deviceAttribute("$localip", m_localip));
                deviceAttributes.emplace_back(deviceAttribute("$mac", m_mac));
                deviceAttributes.emplace_back(deviceAttribute("$fw/name", m_fw_name.toString()));
                deviceAttributes.emplace_back(deviceAttribute("$fw/version", m_fw_version.toString()));
                // TODO: $nodes
                deviceAttributes.emplace_back(deviceAttribute("$implementation", m_implementation));
                deviceAttributes.emplace_back(deviceAttribute("$stats/interval", to_string(m_statsInterval_s)));
                deviceAttributes.emplace_back(deviceAttribute("$state", "ready"));      // TODO: Andere Fälle

                return deviceAttributes;
            }

            protected:
                std::string nameToTopic(const std::string& topic) const {
                    auto convertedTopic = toLower(topic);
                    std::replace( convertedTopic.begin(), convertedTopic.end(), ' ', '-');
                    return convertedTopic;
                }

                std::string macToTopic(const HWInfo& hwInfo) const {
                    auto mac = hwInfo.mac();
                    auto convertedMac = toLower(mac);
                    convertedMac.erase(std::remove(convertedMac.begin(), convertedMac.end(), ':'), convertedMac.end());
                    return convertedMac;
                }

                AttributeType deviceAttribute(const std::string& topic, const std::string& value) {
                    auto deviceTopicPath = std::string{"homie/"} + m_deviceID.toString() + std::string{"/"};
                    return make_pair(deviceTopicPath + topic, value);
                }


                TopicID m_deviceID;
                // $device-attribute
                Version m_homie;
                std::string m_name;
                //TODO: state
                std::string m_localip;
                std::string m_mac;
                TopicID m_fw_name;
                Version m_fw_version;
                //TODO: nodes
                std::string m_implementation;
                //TODO: stats
                uint8_t m_statsInterval_s;

        };

    }
}

#endif /* __HOMIE_DEVICE_H__ */