#ifndef __TEST_NODE_H__
#define __TEST_NODE_H__

#include <Device.h>
#include <AUnit.h>

#include <ArduinoIostream.hpp>

#include "testDevice.h"

namespace Rovi {
    namespace Homie {
        const auto nodeName = std::string{"Car engine"};
        const auto type = std::string{"V8"};

        const auto node = std::make_shared<Node>(nodeName, type);
        const auto nodeArray = std::make_shared<Node>(nodeName, type, 3);
        
        // TODOs:
        // - Node-Pfade
        // - Device <-> Node Verbindung testet
        // - Device-Update inkl. Nodes; Node-Update???
        // - Device.attribute(node) (inkl. arrays)

        test(Homie_Node, attributes) {   
            device->addNode(node);

            auto deviceNode = device->node(node->value(Node::Attributes::nodeID));

            auto mqttRawData = device->connectionInitialized();
            printMqttMessages(mqttRawData);
      
            {
                auto attribute = deviceNode->attribute(Node::Attributes::nodeID);
                assertTrue(attribute.second == std::string{"car-engine"});              
            }
            {
                auto attribute = deviceNode->attribute(Node::Attributes::name);
                assertTrue(attribute.second == std::string{"Car engine"});              
            }
            {
                auto attribute = deviceNode->attribute(Node::Attributes::type);
                assertTrue(attribute.second == std::string{"V8"});              
            }
            {
                // TODO
                auto attribute = deviceNode->attribute(Node::Attributes::properties);
                assertTrue(attribute.second == std::string{"Not implemented yet!"});              
            }
            {
                auto attribute = deviceNode->attribute(Node::Attributes::array);
                assertTrue(attribute.second == std::string{"0"});              
            }
            {
                assertTrue(nodeArray->isArray());
                auto attribute = nodeArray->attribute(Node::Attributes::array);
                assertTrue(attribute.second == std::string{"0-2"});              
            }


        }
    }
}

#endif /* __TEST_NODE_H__ */