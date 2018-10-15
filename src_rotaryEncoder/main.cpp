// TODO:
// - Kombination mit LED  
// - Cleanup (vorallem debugging)

#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

#include <ArduinoOTA.h>

//***************************************************************************//
// Basecamp and Rovi initialize 
//***************************************************************************//
#include <Basecamp.hpp>
// Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
Basecamp iot{
  Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
};

#include "BaseClasses/RoviDevice.hpp"
#include "LED/LEDComponent.hpp"
#include "LED/NeoPixelComponent.hpp"
//#include "LED/RGBLEDComponent.hpp"
#include "Input/RotaryEncoderWithButton.hpp"

RoviDevice myRovi(iot);


// RotaryEncoder
uint8_t pinA = 13;
uint8_t pinB = 12;
uint8_t pinButton = 14;

// LED
uint16_t nbNeoPixelLEDs = 12;
uint8_t  neoPixelPin    = 15;

std::shared_ptr<RotaryEncoderWithButton> rotary;
std::shared_ptr<NeoPixelComponent> leds;
//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  Serial << "--- setup() ---" << endl;

  sleep(5);
  iot.begin();

  myRovi.setupRovi();

  // Setup LED
  leds = std::make_shared<NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin);

  // Setup rotary
    rotary = std::make_shared<RotaryEncoderWithButton>(pinA, pinB, pinButton);
  // NORMAL
  auto normalButtonStateActivatedCallback = []() {
    Serial << "NORMAL state activation callback" << endl;
    if(leds->getPowerStatus()) {
      leds->setPower(false);
      delay(250);
      leds->setPower(true);
    }
  };
  auto normalValueChangeCallback = [&](int value) {
    Serial << "NORMAL value change callback - New value = " << value << endl;

    leds->setBrightness(value * 10);
    leds->setColor(leds->getLastColor());     // Required, because otherwise the color is not restored when changen from brightness 0 to 1...
                                              // TODO: Check, if this is also required and/or working for the RGB_LEDs
  };
  int normalMaxRotaryValue = 25;
  bool normalPreventOverflow = true;
  rotary->setupState(RotaryEncoderWithButton::ButtonStates::NORMAL, normalMaxRotaryValue, normalPreventOverflow, normalButtonStateActivatedCallback, normalValueChangeCallback);

  // CLICKED
  auto clickedButtonStateActivatedCallback = []() {
    Serial << "CLICKED state activation callback" << endl;
    leds->setPower(!leds->getPowerStatus());
  };
  auto clickedValueChangeCallback = [&](int value) {
    Serial << "CLICKED value change callback - Do nothing" << endl;
  };
  int clickedMaxRotaryValue = 10;
  bool clickedPreventOverflow = false;
  rotary->setupState(RotaryEncoderWithButton::ButtonStates::CLICKED, clickedMaxRotaryValue, clickedPreventOverflow, clickedButtonStateActivatedCallback, clickedValueChangeCallback);

  // DOUBLE_CLICKED
  auto doubleClickedButtonStateActivatedCallback = []() {
    Serial << "DOUBLE_CLICKED state activation callback" << endl;
    leds->setPower(false);
    delay(250);
    leds->setPower(true);
    delay(250);
    leds->setPower(false);
    delay(250);
    leds->setPower(true);
  };
  auto doubleClickedValueChangeCallback = [&](int value) {
    Serial << "DOUBLE_CLICKED value change callback - New value = " << value << endl;

    leds->setColor(std::make_shared<HSVColor>(value*10, 1.0f, 1.0f));
  };
  int doubleClickedMaxRotaryValue = 36;
  bool doubleClickedPreventOverflow = false;
  rotary->setupState(RotaryEncoderWithButton::ButtonStates::DOUBLE_CLICKED, doubleClickedMaxRotaryValue, doubleClickedPreventOverflow, doubleClickedButtonStateActivatedCallback, doubleClickedValueChangeCallback);


  myRovi.addComponent(leds);
  // myRovi.addComponent(std::make_shared<RGBLEDComponent>(pinR, pinG, pinB));
  myRovi.addComponent(rotary);

  Serial << "myRovi.baseTopic" << myRovi.getBaseTopic() << endl;

  // TODO: Is there an easier way to connect to methods???
  iot.mqtt.onConnect([&](bool sessionPresent) {myRovi.mqttConnected(sessionPresent);});
  iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {myRovi.mqttSubscribed(packetId, qos);});
  iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
    size_t len, size_t index, size_t total) {myRovi.mqttMessage(topic, payload, properties, len, index, total);});
  iot.mqtt.onPublish([&](uint16_t packetId) {myRovi.mqttPublished(packetId);});


  // Activate Over-the-Air updates
  String otaPassword = iot.configuration.get("OTAPassword");
  Serial << "*******************************************" << endl
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

  rotary->update();

  // sleep(5);
  // // Serial << "--- loop() ---" << endl;
  // // String temp = getTemp();
  // // Serial << "  Temperatur: " << temp << endl;
  // // iot.mqtt.publish("example/temp/", 1, true, temp.c_str());

  // // iot.mqtt.publish("example/foo/", 1, true, "OTA hat geklappt :)");
  // iot.mqtt.publish(myRovi.statusTopic.c_str(), 1, false, "online");

  // Serial << "  myRovi " << myRovi.name  << " is in room " << myRovi.room << endl;
}

 


// Minimal example
// RotaryEncoderWithButton rotary(pinA, pinB, pinButton);

// void setup()
// {
  
//   Serial.begin (115200);
//       delay(10);
//     delay(10);



//   Serial.println("Start");
// }
 
// void loop()
// {
//   rotary.update();

//   delay(10);
// } 

