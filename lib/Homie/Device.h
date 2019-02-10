#ifndef __HOMIE_DEVICE_H__
#define __HOMIE_DEVICE_H__

#include <Arduino.h>
#include <string>
#include <vector>
#include <list>
#include <chrono>
#include <memory>
#include <map>

#include "HomieHelper.h"
#include "Node.h"

namespace Rovi {
    namespace  Homie {

        class TopicID;
        class Version;
        enum class Stats;
        class HWInfo;
        class Node;

        class Device {
            public:
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

                Device(const std::string deviceName, const std::shared_ptr<HWInfo>& hwInfo, 
                    const std::string& firmwareName, const std::shared_ptr<Version>& firmwareVersion,
                    const std::chrono::seconds statsInterval_s);

                // TBD: Visibility 
                std::vector<AttributeType> connectionInitialized();
                std::vector<AttributeType> update() const;

                void addNode(const std::shared_ptr<Node>& node);
                std::shared_ptr<Node> node(const std::string& nodeID) const;

                AttributeType attribute(const Attributes& attribute) const;
                TopicType topic(const Attributes& attribute) const;
                ValueType value(const Attributes& attribute) const;

                AttributeType statictic(const Stats& stat) const;
                TopicType topic(const Stats& stat) const;
                ValueType value(const Stats& stat) const;


                // TBD: Required?
                std::shared_ptr<TopicID> deviceID() const { return m_deviceID; };
                std::shared_ptr<Version> homie() const { return m_homie; };
                std::string name() const { return m_name; };
                State state() const { return m_state; };
                std::string localip() const { return m_localip; };
                std::string mac() const { return m_mac; };
                std::shared_ptr<TopicID> firmwareName() const { return m_fw_name; };
                std::shared_ptr<Version> firmwareVersion() const { return m_fw_version; };
                //TODO: nodes
                std::string implementation() const { return m_implementation; };
                std::chrono::seconds statsInterval_s() const { return m_statsInterval; };

            protected:
                std::string nameToTopic(const std::string& topic) const;
                std::string macToTopic(const std::shared_ptr<HWInfo>& hwInfo) const;
                std::string stateToValue(const State& state) const;
                AttributeType deviceAttribute(const TopicType& topic, const ValueType& value) const;
                std::string availableStatsToValue(const std::list<Stats>& stats) const;

                std::shared_ptr<HWInfo> m_hwInfo;
                std::shared_ptr<TopicID> m_deviceID;
                // $device-attribute
                std::shared_ptr<Version> m_homie;
                std::string m_name;
                State m_state;
                std::string m_localip;
                std::string m_mac;
                std::shared_ptr<TopicID> m_fw_name;
                std::shared_ptr<Version> m_fw_version;
                std::map<std::string, std::shared_ptr<Node>> m_nodes;
                std::string m_implementation;
                std::chrono::seconds m_statsInterval;

                std::list<Stats> m_availableStats;
        };

        // TODO: Move somewhere else
        extern std::string mqttPathToString(const TopicType mqttPath);
        extern void printMqttMessages(const std::vector<AttributeType>& attributes);
    }
}

#endif /* __HOMIE_DEVICE_H__ */