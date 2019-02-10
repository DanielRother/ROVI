#include "Device.h"

#include <utility>
#include <algorithm>

#include <ArduinoIostream.hpp>
#include <FileIOUtils.hpp>

#include "HomieHelper.h"

namespace Rovi {
    namespace  Homie {
        Device::Device(const std::string deviceName, const HWInfo& hwInfo, 
            const std::string& firmwareName, const Version& firmwareVersion,
            const std::chrono::seconds statsInterval)
              : m_hwInfo{hwInfo},
                m_deviceID{nameToTopic(deviceName) + "-" + macToTopic(hwInfo)}, 
                m_homie{3, 0, 1}, m_name{deviceName}, 
                m_state(State::init),
                m_localip{hwInfo.ip()}, m_mac{hwInfo.mac()},
                m_fw_name{firmwareName}, m_fw_version{firmwareVersion}, 
                m_implementation{hwInfo.implementation()}, m_statsInterval{statsInterval}
            {
                m_availableStats = hwInfo.supportedStats();
            }


        std::vector<Device::AttributeType> Device::connectionInitialized()  {
            auto deviceAttributes = std::vector<AttributeType>{};
            deviceAttributes.emplace_back(attribute(Attributes::homie));
            deviceAttributes.emplace_back(attribute(Attributes::name));
            deviceAttributes.emplace_back(attribute(Attributes::localip));
            deviceAttributes.emplace_back(attribute(Attributes::mac));
            deviceAttributes.emplace_back(attribute(Attributes::firmwareName));
            deviceAttributes.emplace_back(attribute(Attributes::firmwareVersion));
            deviceAttributes.emplace_back(attribute(Attributes::nodes));
            deviceAttributes.emplace_back(attribute(Attributes::implementation));
            deviceAttributes.emplace_back(attribute(Attributes::stats));
            deviceAttributes.emplace_back(attribute(Attributes::statsInterval_s));

            m_state = State::ready;
            deviceAttributes.emplace_back(attribute(Attributes::state));      // TODO: Andere Fälle

            return deviceAttributes;
        }

        std::vector<Device::AttributeType> Device::update() const {
            // TODO: Wo wird das Intervall gecheckt?        default = 60

            auto deviceStatistic = std::vector<AttributeType>{};
            for(auto& stat : m_availableStats) {
                deviceStatistic.emplace_back(statictic(stat));
            }
          
            return deviceStatistic;
        }



        Device::AttributeType Device::attribute(const Attributes& attribute) const {
            return deviceAttribute(topic(attribute), value(attribute));
        }


        Device::TopicType Device::topic(const Attributes& attribute) const {
            auto ret = TopicType{};
            switch (attribute)
            {
                case Attributes::deviceID:
                    ret.emplace_back("");       // TODO
                    break;                    
                case Attributes::homie:
                    ret.emplace_back("$homie");
                    break;                    
                case Attributes::name:
                    ret.emplace_back("$name");
                    break;                    
                case Attributes::state:
                    ret.emplace_back("$state");
                    break;                    
                case Attributes::localip:
                    ret.emplace_back("$localip");
                    break;                    
                case Attributes::mac:
                    ret.emplace_back("$mac");
                    break;                    
                case Attributes::firmwareName:
                    ret.emplace_back("$fw");
                    ret.emplace_back("name");
                    break;                    
                case Attributes::firmwareVersion:
                    ret.emplace_back("$fw");
                    ret.emplace_back("version");
                    break;                    
                case Attributes::nodes:
                    ret.emplace_back("$nodes");
                    break;                    
                case Attributes::implementation:
                    ret.emplace_back("$implementation");
                    break;                    
                case Attributes::stats:
                    ret.emplace_back("$stats");
                    break;                    
                case Attributes::statsInterval_s:
                    ret.emplace_back("$stats");
                    ret.emplace_back("interval");
                    break;                    
                default:
                    break;
            }

            return ret;
        }


        Device::ValueType Device::value(const Attributes& attribute) const {
            auto str = ValueType{};
            switch (attribute)
            {
                case Attributes::deviceID:
                    str = m_deviceID.toString();
                    break;                    
                case Attributes::homie:
                    str = m_homie.toString();
                    break;                    
                case Attributes::name:
                    str = m_name;
                    break;                    
                case Attributes::state:
                    str = stateToValue(m_state);
                    break;                    
                case Attributes::localip:
                    str = m_localip;
                    break;                    
                case Attributes::mac:
                    str = m_mac;
                    break;                    
                case Attributes::firmwareName:
                    str = m_fw_name.toString();
                    break;                    
                case Attributes::firmwareVersion:
                    str = m_fw_version.toString();
                    break;                    
                case Attributes::nodes:
                    str = "Not implemented yet!";
                    break;                    
                case Attributes::implementation:
                    str = m_implementation;
                    break;                    
                case Attributes::stats:
                    str = availableStatsToValue(m_availableStats);
                    break;                    
                case Attributes::statsInterval_s:
                    str = to_string(m_statsInterval.count());
                    break;
                default:
                    break;
            }

            return str;
        }

        Device::AttributeType Device::statictic(const Stats& stat) const {
            auto statsBaseTopic = topic(Attributes::stats);
            auto statsSubtopic = topic(stat);
            statsBaseTopic.splice(statsBaseTopic.end(), statsSubtopic);
            return deviceAttribute(statsBaseTopic, value(stat));
        }

        Device::TopicType Device::topic(const Stats& stat) const {
           auto ret = TopicType{};
            switch (stat)
            {
                case Stats::uptime:
                    ret.emplace_back("uptime");
                    break;                    
                case Stats::signal:
                    ret.emplace_back("signal");
                    break;                    
                case Stats::cputemp:
                    ret.emplace_back("cputemp");
                    break;                    
                case Stats::cpuload:
                    ret.emplace_back("cpuload");
                    break;                    
                case Stats::battery:
                    ret.emplace_back("battery");
                    break;                    
                case Stats::freeheap:
                    ret.emplace_back("freeheap");
                    break;                    
                case Stats::supply:
                    ret.emplace_back("supply");
                    break;                               
                default:
                    break;
            }

            return ret;
        }

        Device::ValueType Device::value(const Stats& stat) const {
            auto str = ValueType{};
            switch (stat)
            {
                case Stats::uptime:
                    str = to_string(m_hwInfo.uptime().count());
                    break;
                case Stats::signal:
                    str = to_string(m_hwInfo.signalStrength());
                    break;
                case Stats::cputemp:
                    str = to_string(m_hwInfo.cpuTemperature());
                    break;
                case Stats::cpuload:
                    str = to_string(m_hwInfo.cpuLoad());
                    break;
                case Stats::battery:
                    str = to_string(m_hwInfo.batteryLevel());
                    break;
                case Stats::freeheap:
                    str = to_string(m_hwInfo.freeheap());
                    break;
                case Stats::supply:
                    str = to_string(m_hwInfo.supplyVoltage());
                    break;
                default:
                    break;
            }                  

            return str;
        }


        std::string Device::nameToTopic(const std::string& topic) const {
            auto convertedTopic = toLower(topic);
            std::replace( convertedTopic.begin(), convertedTopic.end(), ' ', '-');
            return convertedTopic;
        }


        std::string Device::macToTopic(const HWInfo& hwInfo) const {
            auto mac = hwInfo.mac();
            auto convertedMac = toLower(mac);
            convertedMac.erase(std::remove(convertedMac.begin(), convertedMac.end(), ':'), convertedMac.end());
            return convertedMac;
        }


        std::string Device::stateToValue(const State& state) const {
            auto str = std::string{};
            switch (state)
            {
                case State::init:
                    str = "init";
                    break;
                case State::ready:
                    str = "ready";
                    break;
                case State::disconnected:
                    str = "disconnected";
                    break;
                case State::sleeping:
                    str = "sleeping";
                    break;
                case State::lost:
                    str = "lost";
                    break;
                case State::alert:
                    str = "alert";
                    break;
            
                default:
                    break;
            }

            return str;
        }


        Device::AttributeType Device::deviceAttribute(const TopicType& topic, const ValueType& value) const {
            auto deviceTopicPath = TopicType{std::string{"homie"}, m_deviceID.toString()};
            deviceTopicPath.insert(deviceTopicPath.end(), topic.begin(), topic.end());
            return make_pair(deviceTopicPath, value);
        }


        std::string Device::availableStatsToValue(const std::list<Stats>& stats) const {
            auto str = std::string{};
            for(auto& stat : stats) {
                auto statTopic = topic(stat);
                if(statTopic.size() != 1) {
                    Serial << "WARNING: statTopic.size() != 1" << endl;
                }
                str += statTopic.front() + ",";
            }
            str.pop_back();        // Remove last ","

            return str;
        }




        //*******************************************************************//
        // TBD
        //*******************************************************************//
        std::string mqttPathToString(const Device::TopicType mqttPath) {
            const auto seperator = std::string{"/"};
            auto path = std::string{};
            for(auto& mqttDir : mqttPath) {
                path += mqttDir + seperator;
            }

            return path;
        }


        void printMqttMessages(const std::vector<Device::AttributeType>& attributes) {
            for(auto& attribute : attributes) {
                auto path = mqttPathToString(attribute.first);
                Serial << path << " -> " << attribute.second << endl;
            }
        }
    }
}