#include "HomieHelper.h"
#include "Device.h"

#include <algorithm>

#include <ArduinoIostream.hpp>
#include <FileIOUtils.hpp>

namespace Rovi {
    namespace Homie {
        //*******************************************************************//
        // TopicID
        //*******************************************************************//
        TopicID::TopicID(const std::string& id) {
            if(isValid(id)) {
                m_id = id;
            } else {
                // TODO?
                Serial << "ERROR: Not a valid topic ID! (id=" << id << ")" << endl;
            }
        }


        // A topic level ID MAY contain lowercase letters from a to z, numbers from 0 to 9 as well as the hyphen character (-).
        // A topic level ID MUST NOT start or end with a hyphen (-). The special character $ is used and reserved for Homie attributes. The underscore (_) is used and reserved for Homie node arrays.
        bool TopicID::isValid(const std::string id) const {
            auto isValid = bool{true};
            isValid &= checkStringForAllowedCharacters(id, std::string("abcdefghijklmnopqrstuvwxyz-01234567890"));
            if(id.size() > 0) {
                isValid &= (id.front() != '-');
                isValid &= (id.back() != '-');
            }

            return isValid;
        }



        //*******************************************************************//
        // VERSION
        //*******************************************************************//
        Version::Version(const uint8_t major, const uint8_t minor, const uint8_t revision) 
            : m_major{major}, m_minor{minor}, m_revision{revision}
        {}


        std::string Version::toString() const {
            return to_string((uint32_t) m_major) + "." + to_string((uint32_t) m_minor) + "." + to_string((uint32_t) m_revision);
        }



        //*******************************************************************//
        // HWInfo
        //*******************************************************************//
        HWInfo::HWInfo(const std::string& mac, const std::string& ip, const std::string& implementation)
            : m_mac{mac}, m_ip{ip}, m_implementation{implementation}
        {} 


        bool HWInfo::supports(const Stats& stat) const {
            auto supported = supportedStats();
            auto it = std::find(supported.begin(), supported.end(), stat);
            return it != supported.end();
        }


        std::list<Stats> HWInfo::supportedStats() const {
            return {Stats::uptime, Stats::signal, Stats::cputemp, Stats::cpuload, Stats::battery, Stats::freeheap, Stats::supply};
        }


        std::chrono::seconds HWInfo::uptime() const {
            return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()- m_start);
        }


        std::string HWInfo::toString() const {
            return std::string{"MAC: "} + m_mac + std::string{", IP: "} + m_ip + std::string{", implementation: "} + m_implementation;
        }


        const std::chrono::system_clock::time_point HWInfo::m_start = std::chrono::system_clock::now();

    }
}