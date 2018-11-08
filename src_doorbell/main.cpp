// TODO:


#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

#include <ArduinoOTA.h>

//***************************************************************************//
// Basecamp and Rovi initialize 
//***************************************************************************//
// #include <Basecamp.hpp>
// // Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
// Basecamp iot{
//   Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
// };

// #include "BaseClasses/RoviDevice.hpp"
// #include "LED/LEDComponent.hpp"
// #include "LED/NeoPixelComponent.hpp"

// RoviDevice myRovi(iot);

// LED
// uint16_t nbNeoPixelLEDs = 20;
// uint8_t  neoPixelPin    = 15;

// std::shared_ptr<NeoPixelComponent> leds;
//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  Serial << "--- setup() ---" << endl;

//   sleep(5);
//   iot.begin();

//   myRovi.setupRovi();

  // Setup LED
//   leds = std::make_shared<NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin);
//   leds->setSwapRGValues(swapRGValues);



//   myRovi.addComponent(leds);

//   Serial << "myRovi.baseTopic" << myRovi.getBaseTopic() << endl;

//   // TODO: Is there an easier way to connect to methods???
//   iot.mqtt.onConnect([&](bool sessionPresent) {myRovi.mqttConnected(sessionPresent);});
//   iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {myRovi.mqttSubscribed(packetId, qos);});
//   iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
//     size_t len, size_t index, size_t total) {myRovi.mqttMessage(topic, payload, properties, len, index, total);});
//   iot.mqtt.onPublish([&](uint16_t packetId) {myRovi.mqttPublished(packetId);});


//   // Activate Over-the-Air updates
//   String otaPassword = iot.configuration.get("OTAPassword");
//   Serial << "*******************************************" << endl
//          << "* OTA PASSWORD:" <<  otaPassword << endl
//          << "*******************************************" << endl;
//   ArduinoOTA.setPassword(otaPassword.c_str());
//   ArduinoOTA.begin();
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
//   ArduinoOTA.handle();

}

