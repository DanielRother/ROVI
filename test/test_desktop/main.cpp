#include <Arduino.h>
#include <AUnit.h>

#include <ArduinoIostream.hpp>
#include "Utils/testFileIOUtils.h"
#include "Homie/testPayloadDataTypes.h"

#include <Device.h>

void setup()
{
  Serial.begin(115200);
  Serial << "Wait for serial monitor setup" << endl;
  sleep(2);
  while(!Serial) {}       // Portability for Leonardo/Micro
  Serial << "Start unit tests" << endl;
}

void loop()
{
  aunit::TestRunner::run();
}