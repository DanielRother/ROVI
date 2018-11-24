// TODO:


#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

#include "DFRobotDFPlayerMini.h"

//***************************************************************************//
// Basecamp and Rovi initialize 
//***************************************************************************//
// #include <Basecamp.hpp>
// // Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
// Basecamp iot{
//   Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
// };
// RoviDevice myRovi(iot);

HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;
uint8_t pinPlayerTX = 12;
uint8_t pinPlayerRX = 13;
void printDetail(uint8_t type, int value);

//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  Serial.begin(115200);
  mySoftwareSerial.begin(9600, SERIAL_8N1, pinPlayerRX, pinPlayerTX); // speed, type, RX, TX
 
  Serial << "--- setup() ---" << endl;

//   sleep(5);
//   iot.begin();


//   Serial << "myRovi.baseTopic" << myRovi.getBaseTopic() << endl;

//   // TODO: Is there an easier way to connect to methods???
//   iot.mqtt.onConnect([&](bool sessionPresent) {myRovi.mqttConnected(sessionPresent);});
//   iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {myRovi.mqttSubscribed(packetId, qos);});
//   iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
//     size_t len, size_t index, size_t total) {myRovi.mqttMessage(topic, payload, properties, len, index, total);});
//   iot.mqtt.onPublish([&](uint16_t packetId) {myRovi.mqttPublished(packetId);});


  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);  //Play the first mp3
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {

}

