#include <Arduino.h>
#include <AUnit.h>

#include <ArduinoIostream.hpp>
#include "Utils/testFileIOUtils.h"
#include "Homie/testPayloadDataTypes.h"

test(ok) 
{
  int x=3;
  int y=3;
  assertEqual(x,y);
}

test(bad)
{
  int x=3;
  int y=3;
  assertNotEqual(x,y);
}

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