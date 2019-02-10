#ifndef __TEST_NODE_H__
#define __TEST_NODE_H__

#include <Device.h>
#include <AUnit.h>

#include "testDevice.h"

namespace Rovi {
    namespace Homie {
        const auto nodeName = std::string{"Car engine"};
        const auto type = std::string{"V8"};

        const auto node = std::make_shared<Node>(nodeName, type);
        const auto nodeArray = Node{nodeName, type, 3};
        

        test(Homie_Node, attributes) {   
            // device.addNode(node);
      
            {
                // auto attribute = node->attribute(Node::Attributes::nodeID);
                // assertTrue(attribute.second == std::string{"car-engine"});
            } 
            // {
            //     auto attribute = device.attribute(Device::Attributes::homie);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$homie/"});
            //     assertTrue(attribute.second == std::string{"3.0.1"});
            // } 
            // {
            //     auto attribute = device.attribute(Device::Attributes::name);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$name/"});
            //     assertTrue(attribute.second == deviceName);
            // } 
            // {
            //     // TODO 
            //     auto attribute = device.attribute(Device::Attributes::state);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$state/"});
            //     assertTrue(attribute.second == std::string{"init"});
            // } 
            // {
            //     auto attribute = device.attribute(Device::Attributes::localip);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$localip/"});
            //     assertTrue(attribute.second == deviceIP);
            // } 
            // {
            //     auto attribute = device.attribute(Device::Attributes::mac);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$mac/"});
            //     assertTrue(attribute.second == deviceMAC);
            // } 
            // {
            //     auto attribute = device.attribute(Device::Attributes::firmwareName);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$fw/name/"});
            //     assertTrue(attribute.second == firmwareName);
            // } 
            // {
            //     auto attribute = device.attribute(Device::Attributes::firmwareVersion);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$fw/version/"});
            //     assertTrue(attribute.second == std::string{"1.0.0"});
            // } 
            // {
            //     // TODO
            //     auto attribute = device.attribute(Device::Attributes::nodes);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$nodes/"});
            //     assertTrue(attribute.second == std::string{"Not implemented yet!"});
            // } 
            // {
            //     auto attribute = device.attribute(Device::Attributes::implementation);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$implementation/"});
            //     assertTrue(attribute.second == implementation);
            // } 
            // {
            //     auto attribute = device.attribute(Device::Attributes::stats);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$stats/"});
            //     Serial << "second: " << attribute.second << endl;
            //     assertTrue(attribute.second == std::string{"uptime,signal,cputemp,cpuload,battery,freeheap,supply"});
            // } 
            // {
            //     auto attribute = device.attribute(Device::Attributes::statsInterval_s);
            //     assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$stats/interval/"});
            //     assertTrue(attribute.second == std::string{"60"});
            // } 
        }
    }
}

#endif /* __TEST_NODE_H__ */