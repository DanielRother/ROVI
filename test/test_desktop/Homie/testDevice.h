#include <Device.h>
#include <AUnit.h>

namespace Rovi {
    namespace Homie {
        test(Homie_Device, foo) {
            auto deviceName = std::string{"Super car"};
            auto hwInfo = HWInfo{"DE:AD:BE:EF:FE:ED", "192.168.0.10", "esp32"};
            auto firmwareName = std::string{"weatherstation-firmware"};
            auto firmwareVersion = Version{1, 0, 0};
            auto statsInterval_s = uint8_t{60};

            auto device = Device(deviceName, hwInfo, firmwareName, firmwareVersion, statsInterval_s);
            auto mqttRawData = device.refresh();

            printMqttMessages(mqttRawData);

            // // String types are limited to 268,435,456 characters
            // // An empty string (“”) is a valid payload
            // auto value = String{""};
            // assertTrue(value.validateValue(""));
            // assertTrue(value.validateValue("abc"));
        }
    }
}