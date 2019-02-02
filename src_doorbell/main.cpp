#include <Arduino.h>
#include <Doorbell.h>

//***************************************************************************//
// Configuration
//***************************************************************************//
uint8_t pinWakeup = 33;
uint8_t playerPinPower = 25;
uint8_t playerPinTX = 26;
uint8_t playerPinRX = 27;

uint8_t playerVolume = 25;
uint8_t playerPlaytime_s = 15;


//***************************************************************************//
// Arduino setup()
//***************************************************************************//
void setup() {
  Serial.begin(115200);
  // sleep(5);
  auto doorbell = Doorbell(playerPinPower, playerPinTX, playerPinRX, playerVolume, pinWakeup);

  // Wait for n seconds before going to sleep
  sleep(playerPlaytime_s);
  doorbell.gotoSleep();
}


//***************************************************************************//
// Arduino loop()
//***************************************************************************//
void loop() {
  Serial.println("Should newer come here");
  sleep(20000);
}