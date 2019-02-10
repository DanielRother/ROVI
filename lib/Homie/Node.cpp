#include "Node.h"

#include <FileIOUtils.hpp>

namespace Rovi {
    namespace Homie {
        Node::Node(const std::shared_ptr<TopicID>& nodeID, const std::string& name, 
            const std::string type, const size_t arraySize)
            : m_nodeID(nodeID), m_name(name), m_type(type), m_arraySize(arraySize) {
            }


        void Node::setDevice(const std::shared_ptr<Device> device) {
            m_device = device;
        }


        std::shared_ptr<Device> Node::device() const {
            return m_device;
        }


        AttributeType Node::attribute(const Attributes& attribute) const {
            return nodeAttribute(topic(attribute), value(attribute));
        }


        TopicType Node::topic(const Attributes& attribute) const {
            auto ret = TopicType{};
            switch (attribute)
            {
                case Attributes::nodeID:
                    ret.emplace_back("");       // TODO
                    break;                                      
                case Attributes::name:
                    ret.emplace_back("$name");
                    break;                    
                case Attributes::type:
                    ret.emplace_back("$type");
                    break;                    
                case Attributes::properties:
                    ret.emplace_back("$properties");
                    break;                    
                case Attributes::array:
                    ret.emplace_back("$array");
                    break;                                     
                default:
                    break;
            }

            return ret;
        }


        ValueType Node::value(const Attributes& attribute) const {
            auto str = ValueType{};
            switch (attribute)
            {
                case Attributes::nodeID:
                    str = m_nodeID->toString();
                    break;                                  
                case Attributes::name:
                    str = m_name;
                    break;                    
                case Attributes::type:
                    str = m_type;
                    break;                    
                case Attributes::properties:
                    str = "Not implemented yet!";
                    break;                    
                case Attributes::array:
                    str = "0";
                    if(isArray()) {
                        str += "-" + to_string(m_arraySize - 1);
                    }
                    break;                
                default:
                    break;
            }

            return str;
        }


        bool Node::isArray() const {
            return m_arraySize > 1;
        }


        AttributeType Node::nodeAttribute(const TopicType& topic, const ValueType& value) const {
            // TODO
            auto deviceTopicPath = TopicType{std::string{"homie"}, m_device->deviceID()->toString()};
            deviceTopicPath.insert(deviceTopicPath.end(), topic.begin(), topic.end());
            return make_pair(deviceTopicPath, value);
        }
            
    }
}