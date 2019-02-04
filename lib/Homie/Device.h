#ifndef __HOMIE_DEVICE_H__
#define __HOMIE_DEVICE_H__

#include <Arduino.h>
#include <string>
#include <vector>
#include <list>
#include <chrono>

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

                // TODO: Implement state changes (requires MQTT...)
                enum class State {
                    init,
                    ready,
                    disconnected,
                    sleeping,
                    lost,
                    alert
                };

                enum class Stats {
                    uptime,
                    signal,
                    cputemp,
                    cpuload,
                    battery,
                    freeheap,
                    supply
                };

                Device(const std::string deviceName, const HWInfo& hwInfo, 
                    const std::string& firmwareName, const Version& firmwareVersion,
                    const std::chrono::seconds statsInterval_s);

                // TBD: Visibility 
                std::vector<AttributeType> connectionInitialized();
                std::vector<AttributeType> update() const;

                AttributeType attribute(const Attributes& attribute) const;
                TopicType topic(const Attributes& attribute) const;
                ValueType value(const Attributes& attribute) const;

                AttributeType statictic(const Stats& stat) const;
                TopicType topic(const Stats& stat) const;
                ValueType value(const Stats& stat) const;


                // TBD: Required?
                TopicID deviceID() const { return m_deviceID; };
                Version homie() const { return m_homie; };
                std::string name() const { return m_name; };
                State state() const { return m_state; };
                std::string localip() const { return m_localip; };
                std::string mac() const { return m_mac; };
                TopicID firmwareName() const { return m_fw_name; };
                Version firmwareVersion() const { return m_fw_version; };
                //TODO: nodes
                std::string implementation() const { return m_implementation; };
                std::chrono::seconds statsInterval_s() const { return m_statsInterval; };
                std::chrono::seconds uptime() const;

            protected:
                std::string nameToTopic(const std::string& topic) const;
                std::string macToTopic(const HWInfo& hwInfo) const;
                std::string stateToValue(const State& state) const;
                AttributeType deviceAttribute(const TopicType& topic, const ValueType& value) const;
                std::string availableStatsToValue(const std::list<Stats>& stats) const;

                HWInfo m_hwInfo;
                TopicID m_deviceID;
                // $device-attribute
                Version m_homie;
                std::string m_name;
                State m_state;
                std::string m_localip;
                std::string m_mac;
                TopicID m_fw_name;
                Version m_fw_version;
                //TODO: nodes
                std::string m_implementation;
                std::chrono::seconds m_statsInterval;

                std::chrono::system_clock::time_point m_start;
                std::list<Stats> m_availableStats;
        };

        // TODO: Move somewhere else
        extern std::string mqttPathToString(const Device::TopicType mqttPath);
        extern void printMqttMessages(const std::vector<Device::AttributeType>& attributes);
    }
}

#endif /* __HOMIE_DEVICE_H__ */