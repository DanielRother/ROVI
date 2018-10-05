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
#include "LED/RGBLEDComponent.hpp"

RoviDevice myRovi(iot);


//***************************************************************************//
// Example method
// To be removed
//***************************************************************************//
int curTemp = 1;
String getTemp() {

  String temp = String(25 + curTemp++);
  curTemp %= 10;

  return temp;
}

//***************************************************************************//
// rotary encoder
//***************************************************************************//
// // Variables
// volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
// volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
// volatile byte encoderPos = 0; //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
// volatile byte oldEncPos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
// volatile byte reading = 0; //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent

// // Callbacks

// void PinA(){
//   cli(); //stop interrupts happening before we read pin values
//   reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
//   if(reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
//     encoderPos --; //decrement the encoder's position count
//     bFlag = 0; //reset flags for the next turn
//     aFlag = 0; //reset flags for the next turn
//   }
//   else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
//   sei(); //restart interrupts
// }

// void PinB(){
//   cli(); //stop interrupts happening before we read pin values
//   reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
//   if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
//     encoderPos ++; //increment the encoder's position count
//     bFlag = 0; //reset flags for the next turn
//     aFlag = 0; //reset flags for the next turn
//   }
//   else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
//   sei(); //restart interrupts
// }

uint8_t pinPressed = 12;
uint8_t pinLeftRotation = 14;
uint8_t pinRightRotation = 27;
#include "AiEsp32RotaryEncoder.h"
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(pinLeftRotation, pinRightRotation, pinPressed, -1);

int test_limits = 2;

void rotary_onButtonClick() {
	//rotaryEncoder.reset();
	//rotaryEncoder.disable();
	rotaryEncoder.setBoundaries(-test_limits, test_limits, false);
	test_limits *= 2;
}

void rotary_loop() {
	//first lets handle rotary encoder button click
	if (rotaryEncoder.currentButtonState() == BUT_RELEASED) {
		//we can process it here or call separate function like:
		rotary_onButtonClick();
	}

	//lets see if anything changed
	int16_t encoderDelta = rotaryEncoder.encoderChanged();
	
	//optionally we can ignore whenever there is no change
	if (encoderDelta == 0) return;
	
	//for some cases we only want to know if value is increased or decreased (typically for menu items)
	if (encoderDelta>0) Serial.print("+");
	if (encoderDelta<0) Serial.print("-");

	//for other cases we want to know what is current value. Additionally often we only want if something changed
	//example: when using rotary encoder to set termostat temperature, or sound volume etc
	
	//if value is changed compared to our last read
	if (encoderDelta!=0) {
		//now we need current value
		int16_t encoderValue = rotaryEncoder.readEncoder();
		//process new value. Here is simple output.
		Serial.print("Value: ");
		Serial.println(encoderValue);
	} 	
}



//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  Serial << "--- setup() ---" << endl;

  sleep(5);
  iot.begin();

  //setting up the rotary encoder
  // pinMode(pinLeftRotation, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  // pinMode(pinRightRotation, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  // attachInterrupt(0,pinLeftRotation,RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  // attachInterrupt(1,pinRightRotation,RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)

  // //setting up rotary encoder push button
  // pinMode(pinPressed, INPUT_PULLUP);
	//we must initialize rorary encoder 
	rotaryEncoder.begin();
	rotaryEncoder.setup([]{rotaryEncoder.readEncoder_ISR();});
	//optionally we can set boundaries and if values should cycle or not
	rotaryEncoder.setBoundaries(0, 10, true); //minValue, maxValue, cycle values (when max go to min and vice versa)


  // myRovi.addComponent(std::make_shared<NeoPixelComponent>(nbNeoPixelLEDs, neoPixelPin));
  // myRovi.addComponent(std::make_shared<RGBLEDComponent>(pinR, pinG, pinB));
  myRovi.setupRovi();

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


  // Noch aus dem Beispiel zum hinzufügen zum Webinterface
  // iot.web.setInterfaceElementAttribute(      // <- Das setzt das Element noch auf Readonly
  //   "tempDisplay",
  //   "readonly",
  //   "true"
  // );

  iot.web.server.on(
    "/temperature",
    HTTP_GET,
    [](AsyncWebServerRequest * request)
  {
    AsyncWebServerResponse *response =
      request->beginResponse(
        200,
        "text/plain",
        getTemp()
      );
    request->send(response);
  });
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
  ArduinoOTA.handle();

  // sleep(5);
  // // Serial << "--- loop() ---" << endl;
  // // String temp = getTemp();
  // // Serial << "  Temperatur: " << temp << endl;
  // // iot.mqtt.publish("example/temp/", 1, true, temp.c_str());

  // // iot.mqtt.publish("example/foo/", 1, true, "OTA hat geklappt :)");
  // iot.mqtt.publish(myRovi.statusTopic.c_str(), 1, false, "online");

  // Serial << "  myRovi " << myRovi.name  << " is in room " << myRovi.room << endl;


  //in loop call your custom function which will process rotary encoder values
	rotary_loop();
	
	delay(50);															 
	if (millis()>20000) rotaryEncoder.enable ();
}