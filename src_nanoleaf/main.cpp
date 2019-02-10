#include <Arduino.h>

#include <ArduinoIostream.hpp>

#include <LED/NeoPixelComponent.hpp>
#include <LED/LEDEffectFactory.hpp>

auto nbPixel = 6;
auto pin = 15;
auto name = "nanoleaf";
auto nanoleaf = NeoPixelComponent{nbPixel, pin, name};
auto colorFlow = LEDEffectFactory::getEffect("color_flow", &nanoleaf);

void setup()
{
  Serial.begin(115200);
  Serial << "Wait for serial monitor setup" << endl;
  sleep(2);
  while(!Serial) {}       // Portability for Leonardo/Micro
  Serial << "Start color flow" << endl;

  nanoleaf.setBrightness(255);
  nanoleaf.setColor(std::make_shared<HSVColor>(128,0.0f,1.0f));
  nanoleaf.setEffect(colorFlow);
}

void loop()
{
    colorFlow->update();
    // Serial << "." << endl;
    // Serial << "RED" << endl;
    // nanoleaf.setColor(std::make_shared<RGBColor>(128,0,0));
    // sleep(1);
    // Serial << "GREEN" << endl;
    // nanoleaf.setColor(std::make_shared<RGBColor>(0,128,0));
    // sleep(1);
    // Serial << "BLUE" << endl;
    // nanoleaf.setColor(std::make_shared<RGBColor>(0,0,128));
    // sleep(1);
    // Serial << "Colorflow" << endl;
    // auto colorFlow = LEDEffectFactory::getEffect("color_flow_fast", &nanoleaf);
    // nanoleaf.setEffect(colorFlow);
    // sleep(5);
}