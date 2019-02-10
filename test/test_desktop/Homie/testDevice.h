#ifndef __TEST_DEVICE_H__
#define __TEST_DEVICE_H__

#include <Device.h>
#include <AUnit.h>

namespace Rovi {
    namespace Homie {
        const auto deviceName = std::string{"Super car"};
        const auto deviceMAC = std::string{"DE:AD:BE:EF:FE:ED"};
        const auto deviceIP = std::string{"192.168.0.10"};
        const auto implementation = std::string{"esp32"};
        const auto firmwareName = std::string{"weatherstation-firmware"};
        const auto firmwareVersion = std::make_shared<Version>(1, 0, 0);
        const auto statsInterval_s = std::chrono::seconds{60};
        const auto hwInfo = std::make_shared<HWInfo>(deviceMAC, deviceIP, implementation);

        auto device = Device(deviceName, hwInfo, firmwareName, firmwareVersion, statsInterval_s);
        const auto baseMqttPath = std::string{"homie/super-car-deadbeeffeed/"};

        test(Homie_Device, attributes) {         
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
                assertTrue(attribute.second == std::string{"init"});
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
                auto attribute = device.attribute(Device::Attributes::stats);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$stats/"});
                Serial << "second: " << attribute.second << endl;
                assertTrue(attribute.second == std::string{"uptime,signal,cputemp,cpuload,battery,freeheap,supply"});
            } 
            {
                auto attribute = device.attribute(Device::Attributes::statsInterval_s);
                assertTrue(mqttPathToString(attribute.first) == baseMqttPath + std::string{"$stats/interval/"});
                assertTrue(attribute.second == std::string{"60"});
            } 
        }

        test(Homie_Device, connectionInitialized) {
            auto mqttRawData = device.connectionInitialized();
            // printMqttMessages(mqttRawData);

            // TODO: Add testcase
            assertTrue(false);
        }

        test(Homie_Device, stateChanges) {
            auto intervall = std::chrono::seconds{60};
            // TODO: Imlement
            assertTrue(false);
        }

        test(Homie_Device, statistic) {
            {
                auto stat = device.statictic(Stats::uptime);
                assertTrue(mqttPathToString(stat.first) == baseMqttPath + std::string{"$stats/uptime/"});
                assertTrue(atoll(stat.second.c_str()) >= 0);
            }  
            {
                auto stat = device.statictic(Stats::signal);
                assertTrue(mqttPathToString(stat.first) == baseMqttPath + std::string{"$stats/signal/"});
                assertTrue(stat.second == "100");
            }  
            {
                auto stat = device.statictic(Stats::cputemp);
                assertTrue(mqttPathToString(stat.first) == baseMqttPath + std::string{"$stats/cputemp/"});
                assertTrue(stat.second == "50");
            }  
            {
                auto stat = device.statictic(Stats::cpuload);
                assertTrue(mqttPathToString(stat.first) == baseMqttPath + std::string{"$stats/cpuload/"});
                assertTrue(stat.second == "0");
            }  
            {
                auto stat = device.statictic(Stats::battery);
                assertTrue(mqttPathToString(stat.first) == baseMqttPath + std::string{"$stats/battery/"});
                assertTrue(stat.second == "100");
            }  
            {
                auto stat = device.statictic(Stats::freeheap);
                assertTrue(mqttPathToString(stat.first) == baseMqttPath + std::string{"$stats/freeheap/"});
                assertTrue(stat.second == "5242880");
            }  
            {
                auto stat = device.statictic(Stats::supply);
                assertTrue(mqttPathToString(stat.first) == baseMqttPath + std::string{"$stats/supply/"});
                assertTrue(stat.second == "3.3");
            }  
        }

        test(Homie_Device, update) {
            // sleep(2);
            auto mqttRawData = device.update();
            // printMqttMessages(mqttRawData);

            assertTrue(false);
        }
    }
}

#endif /* __TEST_DEVICE_H__ */