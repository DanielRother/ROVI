#ifndef __HOMIE_DEVICE_H__
#define __HOMIE_DEVICE_H__

#include <Arduino.h>
#include <string>
#include <vector>
#include <list>

#include "HomieHelper.h"

namespace Rovi {
    namespace  Homie {
        class Device {
            public:
                using TopicType = std::list<std::string>;
                using ValueType = std::string;
                using AttributeType = std::pair<TopicType, ValueType>;

                enum class Attributes {
                    deviceID,
                    homie,
                    name,
                    state,
                    localip,
                    mac,
                    firmwareName,
                    firmwareVersion,
                    nodes,
                    implementation,
                    stats,
                    statsInterval_s
                };

                Device(const std::string deviceName, const HWInfo& hwInfo, 
                    const std::string& firmwareName, const Version& firmwareVersion,
                    const uint8_t statsInterval_s);

                std::vector<AttributeType> refresh() const;     // TBD: name?

                AttributeType attribute(const Attributes& attribute) const;
                TopicType topic(const Attributes& attribute) const;
                ValueType value(const Attributes& attribute) const;

                TopicID deviceID() const { return m_deviceID; };
                Version homie() const { return m_homie; };
                std::string name() const { return m_name; };
                //TODO: state
                std::string localip() const { return m_localip; };
                std::string mac() const { return m_mac; };
                TopicID firmwareName() const { return m_fw_name; };
                Version firmwareVersion() const { return m_fw_version; };
                //TODO: nodes
                std::string implementation() const { return m_implementation; };
                //TODO: stats
                uint8_t statsInterval_s() const { return m_statsInterval_s; };

            protected:
                std::string nameToTopic(const std::string& topic) const;
                std::string macToTopic(const HWInfo& hwInfo) const;
                AttributeType deviceAttribute(const TopicType& topic, const ValueType& value) const;

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

        // TODO: Move somewhere else
        extern std::string mqttPathToString(const Device::TopicType mqttPath);
        extern void printMqttMessages(const std::vector<Device::AttributeType>& attributes);
    }
}

#endif /* __HOMIE_DEVICE_H__ */