#ifndef __HOMIE_NODE_H__
#define __HOMIE_NODE_H__

#include <Arduino.h>

#include <string>
#include <stdint.h>

#include "HomieHelper.h"
#include "Device.h"

namespace Rovi {
    namespace  Homie {

        class TopicID;
        class Version;
        enum class Stats;
        class HWInfo;
        class Device;

        class Node : public std::enable_shared_from_this<Node> {
            public:
                enum class Attributes {
                    nodeID,
                    name,
                    type,
                    properties,
                    array
                };

                Node(const std::string& name, const std::string type, const size_t arraySize);
                Node(const std::string& name, const std::string type);

                void setDevice(const std::shared_ptr<Device> device);
                std::shared_ptr<Device> device() const;

                AttributeType attribute(const Attributes& attribute) const;
                TopicType topic(const Attributes& attribute) const;
                ValueType value(const Attributes& attribute) const;

                bool isArray() const;

            protected:
                AttributeType nodeAttribute(const TopicType& topic, const ValueType& value) const;
                std::string nameToID(const std::string& topic) const;

                std::shared_ptr<TopicID> m_nodeID;
                std::string m_name;
                std::string m_type;
                // TODO: Properties
                size_t m_arraySize;

                std::shared_ptr<Device> m_device;
        };
    }
}

#endif /* __HOMIE_NODE_H__ */