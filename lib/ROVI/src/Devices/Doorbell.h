#ifndef __DOORBELL_H__
#define __DOORBELL_H__

#include <Arduino.h>

#include <string>
#include <stdint.h>

#include <DFRobotDFPlayerMini.h>

#include <esp_sleep.h>
#include <esp_wifi.h>
#include <esp_bt_main.h>
#include <esp_bt.h>
#include <driver/rtc_io.h>

#include <prettyprint.hpp>
#include <ArduinoIostream.hpp>

class Doorbell {
  public:
    Doorbell(const uint8_t playerPinPower, const uint8_t playerPinTX, const uint8_t playerPinRX, 
      const uint8_t playerVolume, const uint8_t pinWakeup)
      : m_serial(SERIAL_UART_NUMBER), m_player(),
        m_playerPinPower(playerPinPower), m_playerPinTX(playerPinTX), m_playerPinRX(playerPinRX),
        m_playerVolume(playerVolume), m_pinWakeup(pinWakeup),
        m_playerInitialized(false)
      {
        Serial << "Initialize doorbell" << endl;
        Serial << "Configuration:" << endl 
          << "  Player pins - Power: " << m_playerPinPower << ", TX: " << m_playerPinTX << ", RX: " << m_playerPinRX << endl
          << "  Volume: " << m_playerVolume << endl
          << "  Wakeup pin: " << m_pinWakeup << endl
          << "  UART: " << SERIAL_UART_NUMBER << endl;
        pinMode(m_playerPinPower, OUTPUT);
        
        Serial << "Activate player power supply" << endl;
        activatePlayerPowerSupply();

        Serial << "Start serial connection to player" << endl;
        m_serial.begin(SERIAL_BAUD_RATE, SERIAL_CONFIG, m_playerPinRX, m_playerPinTX);

        Serial << "Activate player" << endl;
        activatePlayer();
        Serial << "Doorbell setup complete" << endl;
      }
      ~Doorbell() {
        Serial << "Destroy doorbell object" << endl;
        deactivatePlayerPowerSupply();
        gotoSleep();
      }

      void gotoSleep() {
        Serial << endl << "Go to sleep" << endl;
        if(m_playerInitialized) {
          deactivatePlayer();
        }
        deactivatePlayerPowerSupply();

        activateWakeup();
        activateDeepSleep();

        Serial.println("This will also never be printed");
      }

  protected:
    void activatePlayer() {
      activatePlayerPowerSupply();

      // Initialize DFPlayer
      Serial << endl << "DFRobot DFPlayer Mini" << endl;
      Serial << "Initializing DFPlayer ... (May take 3~5 seconds)" << endl;
      auto playerInitOK = m_player.begin(m_serial);
      
      // Error Handling
      if (!playerInitOK) {
        Serial << "Unable to begin:" << endl;
        Serial << "1.Please recheck the connection!" << endl;
        Serial << "2.Please insert the SD card!" << endl;
        m_playerInitialized = false;
        if (m_player.available()) {
            printPlayerInitFailDetail(m_player.readType(), m_player.read()); //Print the detail message from DFPlayer to handle different errors and states.
        }
        gotoSleep();
      } else {
        Serial << "DFPlayer Mini online." << endl;  
        m_playerInitialized = true;
        playSong();
      }
    }

    void deactivatePlayer() {
      Serial << "Stop player" << endl;
      m_player.stop();
      delay(100);

      deactivatePlayerPowerSupply();
    }

    void activatePlayerPowerSupply() {
      Serial << "Activate power supply" << endl;
      digitalWrite(m_playerPinPower, HIGH);
      delay(50);
    }

    void deactivatePlayerPowerSupply() {
      Serial << "Deactivate power supply" << endl;
      digitalWrite(m_playerPinPower, LOW);
    }

    void activateWakeup() {
        pinMode(m_pinWakeup, INPUT_PULLUP);
        rtc_gpio_pullup_en((gpio_num_t) m_pinWakeup);
        esp_sleep_enable_ext0_wakeup((gpio_num_t) m_pinWakeup,0); //1 = High, 0 = Low
    }

    void activateDeepSleep() {
        // Disable some esp component
        esp_bluedroid_disable();
        esp_bt_controller_disable();
        // esp_wifi_stop();  
        // esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF);
        // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
        // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
        // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
        // esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF);

        esp_deep_sleep_start();
        Serial.println("This will never be printed");
    }

    void playSong() {
        m_player.volume(m_playerVolume);

        //Play a random mp3
        auto nbSongs = m_player.readFileCounts();
        auto songID =  random(1, nbSongs + 1);
        m_player.play(songID);
        Serial << "Playing song #" << songID << endl;
    }   

    void printPlayerInitFailDetail(uint8_t type, int value) {
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

    HardwareSerial m_serial;
    DFRobotDFPlayerMini m_player;

    uint8_t m_playerPinPower;
    uint8_t m_playerPinTX;
    uint8_t m_playerPinRX;
    uint8_t m_playerVolume;
    uint8_t m_pinWakeup;

    bool m_playerInitialized;

    static const uint8_t SERIAL_UART_NUMBER = 1;
    static const uint32_t SERIAL_BAUD_RATE = 9600;
    static const uint32_t SERIAL_CONFIG = SERIAL_8N1;
};

#endif /* __DOORBELL_H__ */