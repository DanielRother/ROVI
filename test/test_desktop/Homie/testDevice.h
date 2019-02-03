#include <Device.h>
#include <AUnit.h>

namespace Rovi {
    namespace Homie {
        test(Homie_Device, attributes) {
            auto deviceName = std::string{"Super car"};
            auto deviceMAC = std::string{"DE:AD:BE:EF:FE:ED"};
            auto deviceIP = std::string{"192.168.0.10"};
            auto implementation = std::string{"esp32"};
            auto firmwareName = std::string{"weatherstation-firmware"};
            auto firmwareVersion = Version{1, 0, 0};
            auto statsInterval_s = uint8_t{60};
            auto hwInfo = HWInfo{deviceMAC, deviceIP, implementation};

            auto device = Device(deviceName, hwInfo, firmwareName, firmwareVersion, statsInterval_s);
            
            const auto baseMqttPath = std::string{"homie/super-car-deadbeeffeed/"};
            {
                auto attribute = device.attribute(Device::Attributes::deviceID);
                assertTrue(attribute.second == std::string{"super-car-deadbeeffeed"});
            } 
            {
                auto attribute = device.attribute(Device::Attributes::homie);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$homie/"});
                assertTrue(attribute.second == std::string{"3.0.1"});
            } 
            {
                auto attribute = device.attribute(Device::Attributes::name);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$name/"});
                assertTrue(attribute.second == deviceName);
            } 
            {
                // TODO 
                auto attribute = device.attribute(Device::Attributes::state);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$state/"});
                assertTrue(attribute.second == std::string{"Not implemented yet!"});
            } 
            {
                auto attribute = device.attribute(Device::Attributes::localip);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$localip/"});
                assertTrue(attribute.second == deviceIP);
            } 
            {
                auto attribute = device.attribute(Device::Attributes::mac);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$mac/"});
                assertTrue(attribute.second == deviceMAC);
            } 
            {
                auto attribute = device.attribute(Device::Attributes::firmwareName);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$fw/name/"});
                assertTrue(attribute.second == firmwareName);
            } 
            {
                auto attribute = device.attribute(Device::Attributes::firmwareVersion);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$fw/version/"});
                assertTrue(attribute.second == std::string{"1.0.0"});
            } 
            {
                // TODO
                auto attribute = device.attribute(Device::Attributes::nodes);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$nodes/"});
                assertTrue(attribute.second == std::string{"Not implemented yet!"});
            } 
            {
                auto attribute = device.attribute(Device::Attributes::implementation);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$implementation/"});
                assertTrue(attribute.second == implementation);
            } 
            {
                // TODO:
                auto attribute = device.attribute(Device::Attributes::stats);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$stats/"});
                assertTrue(attribute.second == std::string{"Not implemented yet!"});
            } 
            {
                auto attribute = device.attribute(Device::Attributes::statsInterval_s);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$stats/interval/"});
                assertTrue(attribute.second == std::string{"60"});
            } 
        }

        test(Homie_Device, refresh) {
            auto deviceName = std::string{"Super car"};
            auto deviceMAC = std::string{"DE:AD:BE:EF:FE:ED"};
            auto deviceIP = std::string{"192.168.0.10"};
            auto implementation = std::string{"esp32"};
            auto firmwareName = std::string{"weatherstation-firmware"};
            auto firmwareVersion = Version{1, 0, 0};
            auto statsInterval_s = uint8_t{60};
            auto hwInfo = HWInfo{deviceMAC, deviceIP, implementation};

            auto device = Device(deviceName, hwInfo, firmwareName, firmwareVersion, statsInterval_s);

            auto mqttRawData = device.refresh();
            // printMqttMessages(mqttRawData);

            // TODO: Add testcase
        }
    }
}