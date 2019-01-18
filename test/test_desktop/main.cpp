#include <Arduino.h>
#include <ArduinoUnit.h>

#include "Utils/testFileIOUtils.h"

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
  sleep(2);               // To allow serial monitor setup
  while(!Serial) {}       // Portability for Leonardo/Micro
}

void loop()
{
  Test::run();
}