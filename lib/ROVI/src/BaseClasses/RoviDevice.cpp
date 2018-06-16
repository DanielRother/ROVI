#include "BaseClasses/RoviDevice.hpp"

#include <prettyprint.hpp>
#include <arduinoIostream.hpp>

RoviDevice::RoviDevice(Basecamp& iot)
    : iot(iot), name(""), room("")
{}

void RoviDevice::setupRovi() {
    Serial << "--- RoviDevice::setupRovi() ---" << endl;
    // Needs to be in a dedicated method, 'cause iot.begin() in the ctor crashes the application...
    // iot.begin();

    name = iot.configuration.get("DeviceName");
    room = addVariableToIotConfig("room", "/haus/etage/zimmer");
    iot.configuration.save();
}

//****************************************//
// MQTT interface methods
//****************************************//
void RoviDevice::mqttConnected(bool sessionPresent) {

}

void RoviDevice::mqttSubscribed(uint16_t packetId, uint8_t qos) {

}

void RoviDevice::mqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {

}

void RoviDevice::mqttPublished(uint16_t packetId) {

}

// private:
String RoviDevice::addVariableToIotConfig(String name, String defaultValue) {
    String var = iot.configuration.get(name);

    if(var.length() == 0) {
        Serial << "  Variable '" << name << "' is empty. Set to default (" << defaultValue << ")\n";
        iot.configuration.set(name, defaultValue);
    } else {
        Serial << "  Variable '" << name << "': " << var << endl;
    }

    iot.web.addInterfaceElement(
        "html_" + name,
        "input",
        "Variable " + name + ":",
        "#configform",
        name
    );

    return iot.configuration.get(name);
}