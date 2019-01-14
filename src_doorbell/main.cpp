// // TODO:


#include <Arduino.h>

#include <esp_sleep.h>
#include <esp_wifi.h>
#include <esp_bt_main.h>
#include <esp_bt.h>
#include <driver/rtc_io.h>

#include <string>
#include <stdint.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

#include <DFRobotDFPlayerMini.h>

#include "PayloadDataTypes.h"

// //***************************************************************************//
// // Basecamp and Rovi initialize 
// //***************************************************************************//
// #include <Basecamp.hpp>
// #include <BaseClasses/RoviDevice.hpp>
// #include <EnumUtils.hpp>
// // Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
// Basecamp iot{
//   Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
// };
// RoviDevice myRovi(iot);

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
  // Just testing Homie... Move somewhere else!!!
  auto s = Rovi::Homie::String{"abcd"};

  Serial.begin(115200);
  mySoftwareSerial.begin(9600, SERIAL_8N1, pinPlayerRX, pinPlayerTX); // speed, type, RX, TX
 
//   sleep(5);
  // Activate power supply of DFRobot
  pinMode(pinPower, OUTPUT);
  digitalWrite(pinPower, HIGH);

  delay(50);
 
  Serial << endl << "DFRobot DFPlayer Mini Demo" << endl;
  Serial << "Initializing DFPlayer ... (May take 3~5 seconds)" << endl;
  
  //TODO: Endlosschleife an dieser Stelle verhindern!
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial << "Unable to begin:" << endl;
    Serial << "1.Please recheck the connection!" << endl;
    Serial << "2.Please insert the SD card!" << endl;

    digitalWrite(pinPower, LOW);

    // Go to sleep
    esp_bluedroid_disable();
    esp_bt_controller_disable();

    // Activate Wakeup pin
    pinMode(pinWakeup, INPUT_PULLUP);
    rtc_gpio_pullup_en(pinWakeup);
    esp_sleep_enable_ext0_wakeup(pinWakeup,0); //1 = High, 0 = Low
    Serial << "Player stoped" << endl;
    esp_deep_sleep_start();
    Serial.println("This will never be printed");

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

    
  // Serial << "Start MQTT/Rovi" << endl;
  // iot.begin();
  // myRovi.setupRovi();
  // Serial << "myRovi.baseTopic" << myRovi.getBaseTopic() << endl;

  // // TODO: Is there an easier way to connect to methods???
  // iot.mqtt.onConnect([&](bool sessionPresent) {myRovi.mqttConnected(sessionPresent);});
  // iot.mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {
  //   myRovi.mqttSubscribed(packetId, qos);
  //   iot.mqtt.publish((myRovi.getBaseTopic() + "/status").c_str(), 
  //   to_underlying(RoviDevice::MQTTQoSClasses::AT_MOST_ONE), false, 
  //   "dingdong");
  //   });
  // iot.mqtt.onMessage([&](char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
  //   size_t len, size_t index, size_t total) {myRovi.mqttMessage(topic, payload, properties, len, index, total);});
  // iot.mqtt.onPublish([&](uint16_t packetId) {myRovi.mqttPublished(packetId);});

  // Wait for n seconds before going to sleep
  sleep(15);
  Serial << "Go to sleep - Stop player" << endl;
  myDFPlayer.stop();
  delay(100);
  
  // Deactivate power supply of DFRobot
  digitalWrite(pinPower, LOW);
  delay(20);

  // Go to sleep
  esp_bluedroid_disable();
  esp_bt_controller_disable();
  // esp_wifi_stop();  
  // esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF);
  delay(100);

  // esp_sleep_disable_wakeup_source(esp_sleep_source_t::);
  // Activate Wakeup pin
  pinMode(pinWakeup, INPUT_PULLUP);
  rtc_gpio_pullup_en(pinWakeup);
  esp_sleep_enable_ext0_wakeup(pinWakeup,0); //1 = High, 0 = Low
  Serial << "Player stoped" << endl;
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
  // Go to sleep
  esp_bluedroid_disable();
  esp_bt_controller_disable();

  // Activate Wakeup pin
  pinMode(pinWakeup, INPUT_PULLUP);
  rtc_gpio_pullup_en(pinWakeup);
  esp_sleep_enable_ext0_wakeup(pinWakeup,0); //1 = High, 0 = Low
  Serial << "Player stoped" << endl;
  esp_deep_sleep_start();
  Serial.println("This will never be printed");



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


// // Based on: https://gist.github.com/ItHasU/6cfdc8160ca3bf9407ef351b1a6454d7
// #include <esp_wifi.h>

// #define uS_TO_S_FACTOR 1000000 /* Conversion factor for u seconds to seconds */
// #define TIME_TO_SLEEP  10      /* Time ESP32 will go to sleep (in seconds) */

// //const int PIN_LED = 22; // LOLIN32 lite
// const int PIN_LED = 5;  // LOLIN32

// void setup()
// {
//   pinMode(PIN_LED, OUTPUT);
//   digitalWrite(PIN_LED, LOW);
// }

// void loop()
// {
//   Serial.println("Start loop");
//   delay(TIME_TO_SLEEP * 1000);

//   Serial.println("Go to sleep");
//   esp_wifi_deinit();
//   esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
//   esp_deep_sleep_start();

//   Serial.println("Never printed");
// }



// /*
// Simple Deep Sleep with Timer Wake Up
// =====================================
// ESP32 offers a deep sleep mode for effective power
// saving as power is an important factor for IoT
// applications. In this mode CPUs, most of the RAM,
// and all the digital peripherals which are clocked
// from APB_CLK are powered off. The only parts of
// the chip which can still be powered on are:
// RTC controller, RTC peripherals ,and RTC memories
// This code displays the most basic deep sleep with
// a timer to wake it up and how to store data in
// RTC memory to use it over reboots
// This code is under Public Domain License.
// Author:
// Pranav Cherukupalli <cherukupallip@gmail.com>
// */

// #define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
// #define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

// RTC_DATA_ATTR int bootCount = 0;

// /*
// Method to print the reason by which ESP32
// has been awaken from sleep
// */
// void print_wakeup_reason(){
//   esp_sleep_wakeup_cause_t wakeup_reason;

//   wakeup_reason = esp_sleep_get_wakeup_cause();

//   switch(wakeup_reason)
//   {
//     case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
//     case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
//     case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
//     case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
//     case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
//     default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
//   }
// }

// void setup(){
//   Serial.begin(115200);
//   delay(1000); //Take some time to open up the Serial Monitor

//   //Increment boot number and print it every reboot
//   ++bootCount;
//   Serial.println("Boot number: " + String(bootCount));

//   //Print the wakeup reason for ESP32
//   print_wakeup_reason();

//   /*
//   First we configure the wake up source
//   We set our ESP32 to wake up every 5 seconds
//   */
//   esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
//   Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
//   " Seconds");

//   /*
//   Next we decide what all peripherals to shut down/keep on
//   By default, ESP32 will automatically power down the peripherals
//   not needed by the wakeup source, but if you want to be a poweruser
//   this is for you. Read in detail at the API docs
//   http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
//   Left the line commented as an example of how to configure peripherals.
//   The line below turns off all RTC peripherals in deep sleep.
//   */
//   //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
//   //Serial.println("Configured all RTC Peripherals to be powered down in sleep");

//   /*
//   Now that we have setup a wake cause and if needed setup the
//   peripherals state in deep sleep, we can now start going to
//   deep sleep.
//   In the case that no wake up sources were provided but deep
//   sleep was started, it will sleep forever unless hardware
//   reset occurs.
//   */
//   Serial.println("Going to sleep now");
//   Serial.flush(); 
//   esp_deep_sleep_start();
//   Serial.println("This will never be printed");
// }

// void loop(){
//   //This is not going to be called
// }


// /*
// Deep Sleep with External Wake Up
// =====================================
// This code displays how to use deep sleep with
// an external trigger as a wake up source and how
// to store data in RTC memory to use it over reboots

// This code is under Public Domain License.

// Hardware Connections
// ======================
// Push Button to GPIO 33 pulled down with a 10K Ohm
// resistor

// NOTE:
// ======
// Only RTC IO can be used as a source for external wake
// source. They are pins: 0,2,4,12-15,25-27,32-39.

// Author:
// Pranav Cherukupalli <cherukupallip@gmail.com>
// */

// #define BUTTON_PIN_BITMASK 0x200000000 // 2^33 in hex

// RTC_DATA_ATTR int bootCount = 0;

// /*
// Method to print the reason by which ESP32
// has been awaken from sleep
// */
// void print_wakeup_reason(){
//   esp_sleep_wakeup_cause_t wakeup_reason;

//   wakeup_reason = esp_sleep_get_wakeup_cause();

//   switch(wakeup_reason)
//   {
//     case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
//     case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
//     case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
//     case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
//     case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
//     default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
//   }
// }

// void setup(){
//   Serial.begin(115200);
//   delay(1000); //Take some time to open up the Serial Monitor

//   //Increment boot number and print it every reboot
//   ++bootCount;
//   Serial.println("Boot number: " + String(bootCount));

//   //Print the wakeup reason for ESP32
//   print_wakeup_reason();

//   /*
//   First we configure the wake up source
//   We set our ESP32 to wake up for an external trigger.
//   There are two types for ESP32, ext0 and ext1 .
//   ext0 uses RTC_IO to wakeup thus requires RTC peripherals
//   to be on while ext1 uses RTC Controller so doesnt need
//   peripherals to be powered on.
//   Note that using internal pullups/pulldowns also requires
//   RTC peripherals to be turned on.
//   */
//   esp_sleep_enable_ext0_wakeup(GPIO_NUM_34,1); //1 = High, 0 = Low

//   //If you were to use ext1, you would use it like
//   //esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

//   //Go to sleep now
//   Serial.println("Going to sleep now");
//   esp_deep_sleep_start();
//   Serial.println("This will never be printed");
// }

// void loop(){
//   //This is not going to be called
// }
