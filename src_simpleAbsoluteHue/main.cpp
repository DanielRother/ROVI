#include <Arduino.h>
#include <ArduinoOTA.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>


//***************************************************************************//
// Basecamp and Rovi initialize 
//***************************************************************************//
#include <Basecamp.hpp>
// Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
Basecamp iot{
  Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
};

#include "BaseClasses/RoviDevice.hpp"

#include <Devices/SimpleAbsoluteHue.h>


Rovi::RoviDevice myRovi(iot);


// RotaryEncoder
uint8_t pinA = 12;
uint8_t pinB = 13;
uint8_t pinButton = 14;

// LED
uint16_t nbNeoPixelLEDs = 20;
uint8_t  neoPixelPin    = 15;

std::shared_ptr<Rovi::Devices::SimpleAbsoluteHue> absolutHue;
//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  // TODO:
  //    Test:
  //        RotaryEncoder
  //        LEDFactory

    std::cout << "--- setup() ---" << endl;

    sleep(5);
    iot.begin();

    myRovi.setupRovi();

    absolutHue = std::make_shared<Rovi::Devices::SimpleAbsoluteHue>(pinA, pinB, pinButton,
                                                                    neoPixelPin, nbNeoPixelLEDs);

  // // Setup LED
  // leds = std::make_shared<NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin);
  // auto swapRGValues = std::vector<uint32_t>(nbNeoPixelLEDs, 0);
  // for(size_t pixelIdx = 0; pixelIdx < 12; ++pixelIdx) {
  //   swapRGValues[pixelIdx] = 1;
  // }
  // leds->setSwapRGValues(swapRGValues);



  std::cout << "myRovi.baseTopic" << myRovi.getBaseTopic() << endl;
  // TODO: Is there an easier way to connect to methods???
  iot.mqtt.onConnect([&](bool sessionPresent) {myRovi.mqttConnected(sessionPresent);});
  iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {myRovi.mqttSubscribed(packetId, qos);});
  iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
    size_t len, size_t index, size_t total) {myRovi.mqttMessage(topic, payload, properties, len, index, total);});
  iot.mqtt.onPublish([&](uint16_t packetId) {myRovi.mqttPublished(packetId);});


  // Activate Over-the-Air updates
  String otaPassword = iot.configuration.get("OTAPassword");
  std::cout << "*******************************************" << endl
         << "* OTA PASSWORD:" <<  otaPassword << endl
         << "*******************************************" << endl;
  ArduinoOTA.setPassword(otaPassword.c_str());
  ArduinoOTA.begin();
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
bool showEnableMessage = false;

void loop() {
  ArduinoOTA.handle();

  absolutHue->update();
}

