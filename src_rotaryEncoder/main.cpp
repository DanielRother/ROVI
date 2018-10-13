#include <Arduino.h>

// #include <string>
// #include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

// #include <ArduinoOTA.h>

// //***************************************************************************//
// // Basecamp and Rovi initialize 
// //***************************************************************************//
// #include <Basecamp.hpp>
// // Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
// Basecamp iot{
//   Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
// };

// #include "BaseClasses/RoviDevice.hpp"
// #include "LED/LEDComponent.hpp"
// #include "LED/NeoPixelComponent.hpp"
// #include "LED/RGBLEDComponent.hpp"

// RoviDevice myRovi(iot);





// //***************************************************************************//
// // Arduino setup()
// //***************************************************************************//
// void setup() {
//   Serial << "--- setup() ---" << endl;

//   sleep(5);
//   iot.begin();

//   // myRovi.addComponent(std::make_shared<NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin));
//   // myRovi.addComponent(std::make_shared<RGBLEDComponent>(pinR, pinG, pinB));
//   myRovi.setupRovi();

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


//   // Noch aus dem Beispiel zum hinzufügen zum Webinterface
//   // iot.web.setInterfaceElementAttribute(      // <- Das setzt das Element noch auf Readonly
//   //   "tempDisplay",
//   //   "readonly",
//   //   "true"
//   // );

//   iot.web.server.on(
//     "/temperature",
//     HTTP_GET,
//     [](AsyncWebServerRequest * request)
//   {
//     AsyncWebServerResponse *response =
//       request->beginResponse(
//         200,
//         "text/plain",
//         getTemp()
//       );
//     request->send(response);
//   });
// }


// //***************************************************************************//
// // Arduino loop()
// //***************************************************************************//
// bool showEnableMessage = false;

// void loop() {
//   ArduinoOTA.handle();

//   // sleep(5);
//   // // Serial << "--- loop() ---" << endl;
//   // // String temp = getTemp();
//   // // Serial << "  Temperatur: " << temp << endl;
//   // // iot.mqtt.publish("example/temp/", 1, true, temp.c_str());

//   // // iot.mqtt.publish("example/foo/", 1, true, "OTA hat geklappt :)");
//   // iot.mqtt.publish(myRovi.statusTopic.c_str(), 1, false, "online");

//   // Serial << "  myRovi " << myRovi.name  << " is in room " << myRovi.room << endl;
// }


#include <Basecamp.hpp>       // MUST be include bevore any ROVI header...! Check why? (Including Basecamp.hppp within the Rovi classes doesn't work)
#include "Input/RotaryEncoderWithButton.hpp"
// #include "RotaryEncoderWithButton.hpp"
uint8_t pinA = 13;
uint8_t pinB = 12;
uint8_t pinButton = 15;

RotaryEncoderWithButton rotary(pinA, pinB, pinButton);
 
// TODO:
// - OneButton-Forken
// - Integration into ROVI
// - MQTT-Messages
// - Kombination mit LED  
void setup()
{
  
  Serial.begin (115200);
      delay(10);
    delay(10);


  // Test setup some state
  auto buttonStateActivatedCallback = []() {
    Serial << "My user defined state activation callback" << endl;
  };
  auto valueChangeCallback = [&](int value) {
    Serial << "My user defined value change callback - New value = " << value << endl;
  };
  int maxRotaryValue = 10;
  bool preventOverflow = false;
  rotary.setupState(RotaryEncoderWithButton::ButtonStates::CLICKED, maxRotaryValue, preventOverflow, buttonStateActivatedCallback, valueChangeCallback);

  Serial.println("Start");
}
 
void loop()
{
  rotary.update();

  delay(10);
} 

