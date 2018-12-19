// TODO:


#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

#include <DFRobotDFPlayerMini.h>

//***************************************************************************//
// Basecamp and Rovi initialize 
//***************************************************************************//
#include <Basecamp.hpp>
#include <BaseClasses/RoviDevice.hpp>
#include <EnumUtils.hpp>
// Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
Basecamp iot{
  Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
};
RoviDevice myRovi(iot);

HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;
uint8_t pinPlayerTX = 26;
uint8_t pinPlayerRX = 27;
uint8_t pinPower    = 25;
auto pinWakeup   = GPIO_NUM_33;
void printDetail(uint8_t type, int value);

//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  Serial.begin(115200);
  mySoftwareSerial.begin(9600, SERIAL_8N1, pinPlayerRX, pinPlayerTX); // speed, type, RX, TX
 
//   sleep(5);
  // Activate power supply of DFRobot
  pinMode(pinPower, OUTPUT);
  digitalWrite(pinPower, HIGH);

  // Activate Wakeup pin
  pinMode(pinWakeup, INPUT_PULLUP);
  delay(50);
 
  Serial << endl << "DFRobot DFPlayer Mini Demo" << endl;
  Serial << "Initializing DFPlayer ... (May take 3~5 seconds)" << endl;
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial << "Unable to begin:" << endl;
    Serial << "1.Please recheck the connection!" << endl;
    Serial << "2.Please insert the SD card!" << endl;
    delay(100);
    while(true);
  }
  Serial << "DFPlayer Mini online." << endl;
  
  myDFPlayer.volume(25);  //Set volume value. From 0 to 30

  //Play a random mp3
  auto nbSongs = myDFPlayer.readFileCounts();
  auto songID =  random(1, nbSongs + 1);
  myDFPlayer.play(songID);
  Serial << "Playing song #" << songID << endl;

    
  Serial << "Start MQTT/Rovi" << endl;
  iot.begin();
  myRovi.setupRovi();
  Serial << "myRovi.baseTopic" << myRovi.getBaseTopic() << endl;

  // TODO: Is there an easier way to connect to methods???
  iot.mqtt.onConnect([&](bool sessionPresent) {myRovi.mqttConnected(sessionPresent);});
  iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {
    myRovi.mqttSubscribed(packetId, qos);
    iot.mqtt.publish((myRovi.getBaseTopic() + "/status").c_str(), 
    to_underlying(RoviDevice::MQTTQoSClasses::AT_MOST_ONE), false, 
    "dingdong");
    });
  iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
    size_t len, size_t index, size_t total) {myRovi.mqttMessage(topic, payload, properties, len, index, total);});
  iot.mqtt.onPublish([&](uint16_t packetId) {myRovi.mqttPublished(packetId);});

  // Wait for n seconds before going to sleep
  sleep(15);
  Serial << "Go to sleep - Stop player" << endl;
  myDFPlayer.stop();
  delay(100);
  
  // Deactivate power supply of DFRobot
  digitalWrite(pinPower, LOW);
  delay(20);

  // Go to sleep
  esp_sleep_enable_ext0_wakeup(pinWakeup,0); //1 = High, 0 = Low
  Serial << "Player stoped" << endl;
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
  //This is not going to be called

  // static unsigned long timer = millis();
  
  // if (millis() - timer > 10000) {
  //   timer = millis();
  //   myDFPlayer.next();  //Play next mp3 every 3 second.
  // }
  
  // if (myDFPlayer.available()) {
  //   printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  // }
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

