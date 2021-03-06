#include <Arduino.h>

#include <ArduinoIostream.hpp>

// #include <ArduinoOTA.h>

// //***************************************************************************//
// // Basecamp and Rovi initialize
// //***************************************************************************//
// #include <Basecamp.hpp>
// // Basecamp wird angewiesen einen verschlüsselten Acess-Point zu öffnen. Das Passwort erscheint in der seriellen Konsole.
// Basecamp iot{
//   Basecamp::SetupModeWifiEncryption::secured, Basecamp::ConfigurationUI::always
// };

// #include <BaseClasses/RoviDevice.hpp>
// #include <Components/Output/NeoPixelComponent.hpp>
// #include <Common/LED/LEDEffectFactory.hpp>

// Rovi::Deprecated::RoviDevice myRovi(iot);

// auto nbPixel = 6;
// auto pin = 15;
// auto name = "nanoleaf";
// // auto nanoleaf = Rovi::Components::NeoPixelComponent{nbPixel, pin, name};
// auto nanoleaf = std::make_shared<Rovi::Components::NeoPixelComponent>(nbPixel, pin, name);
// auto colorFlow = Rovi::LEDEffectFactory::getEffect("color_flow", nanoleaf.get());

// void setup()
// {
//   std::cout << "--- setup() ---" << endl;
//   sleep(5);
//   iot.begin();
//   myRovi.setupRovi();

//   // Activate Over-the-Air updates
//   String otaPassword = iot.configuration.get("OTAPassword"); // The prefedined variable OTAPassword is always reset to '1'; TODO: Check why/fix?
//   std::cout << "*******************************************" << endl
//          << "* OTA PASSWORD:" <<  otaPassword.c_str() << endl
//          << "*******************************************" << endl;

//   ArduinoOTA.setPassword(otaPassword.c_str());
//   ArduinoOTA.begin();

//   // TODO: Init Neopixel
//   // TODO: Move to dedicated component
//   std::cout << "Init neopixel" << std::endl;
//   nanoleaf->setBrightness(255);
//   nanoleaf->setColor(std::make_shared<Rovi::HSVColor>(128,0.0f,0.5f));
//   Serial << "Start color flow" << endl;
//   nanoleaf->setEffect(colorFlow);
// }

// void loop()
// {
//     ArduinoOTA.handle();

//     nanoleaf->update();

//     // Serial << "." << endl;
//     // Serial << "RED" << endl;
//     // nanoleaf.setColor(std::make_shared<RGBColor>(128,0,0));
//     // sleep(1);
//     // Serial << "GREEN" << endl;
//     // nanoleaf.setColor(std::make_shared<RGBColor>(0,128,0));
//     // sleep(1);
//     // Serial << "BLUE" << endl;
//     // nanoleaf.setColor(std::make_shared<RGBColor>(0,0,128));
//     // sleep(1);
//     // Serial << "Colorflow" << endl;
//     // auto colorFlow = LEDEffectFactory::getEffect("color_flow_fast", &nanoleaf);
//     // nanoleaf.setEffect(colorFlow);
//     // sleep(5);
// }

#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>
// #include <base64.h>
extern "C" {
#include "crypto/base64.h"
}
#include <pngle.h>

#include <iostream>
#include <vector>
#include <string>

#include "exampleColors.h"

const auto nbRows = 8;
const auto nbColumns = 32;
const auto pinMatrix = 13;
const auto nbPixel = nbColumns * nbRows;
const auto brightness = 16;

CRGB leds[nbPixel];
// Define matrix width and height.
auto matrix = new FastLED_NeoMatrix(leds, nbColumns, nbRows,
                                    NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
                                        NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);

// const uint16_t colors[] = {
//   matrix->Color(255, 0, 0), matrix->Color(0, 255, 0), matrix->Color(0, 0, 255) };

auto myColors = std::vector<uint16_t>{matrix->Color(255, 0, 0), matrix->Color(0, 255, 0), matrix->Color(0, 0, 255)};
auto colorIdx = 0;

uint16_t getNextColor()
{
  colorIdx = (colorIdx + 1) % myColors.size();
  auto nextColor = myColors[colorIdx];
  std::cout << "Next color: " << nextColor << std::endl;
  return nextColor;
}

void display_scrollTextOrig()
{
  uint8_t size = max(int(nbColumns / 8), 1);
  matrix->clear();
  matrix->setTextWrap(false); // we don't wrap text so it scrolls nicely
  matrix->setTextSize(1);
  matrix->setRotation(0);
  for (int8_t x = 7; x >= -42; x--)
  {
    yield();
    matrix->clear();
    matrix->setCursor(x, 0);
    matrix->setTextColor(LED_GREEN_HIGH);
    matrix->print("Hello");
    if (nbRows > 11)
    {
      matrix->setCursor(-20 - x, nbRows - 7);
      matrix->setTextColor(LED_ORANGE_HIGH);
      matrix->print("World");
    }
    matrix->show();
    delay(50);
  }

  matrix->setRotation(3);
  matrix->setTextSize(size);
  matrix->setTextColor(LED_BLUE_HIGH);
  for (int16_t x = 8 * size; x >= -6 * 8 * size; x--)
  {
    yield();
    matrix->clear();
    matrix->setCursor(x, nbColumns / 2 - size * 4);
    matrix->print("Rotate");
    matrix->show();
    // note that on a big array the refresh rate from show() will be slow enough that
    // the delay become irrelevant. This is already true on a 32x32 array.
    delay(50 / size);
  }
  matrix->setRotation(0);
  matrix->setCursor(0, 0);
  matrix->show();
}

void display_runningPixel()
{
  auto curColor = getNextColor();

  for (auto c = 0; c < nbColumns; ++c)
  {
    for (auto r = 0; r < nbRows; ++r)
    {
      std::cout << "c = " << c << ", r = " << r << std::endl;
      matrix->drawPixel(c, r, curColor);
      matrix->show();
      delay(100);
      matrix->drawPixel(c, r, matrix->Color(0, 0, 0));
      matrix->show();
    }
  }
}

void display_scrollText(const std::string &text)
{
  std::cout << "display scrollText " << text << std::endl;
  const auto charSize = 6;
  const auto xLimit = -1 * (int)text.size() * charSize;
  std::cout << "xLimit = " << xLimit << std::endl;

  matrix->setTextColor(getNextColor());
  for (auto x = matrix->width(); x > xLimit; --x)
  {
    matrix->fillScreen(0);
    matrix->setCursor(x, 0);
    matrix->print(F(text.c_str()));
    matrix->show();
    delay(100);
  }
}

void on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
    uint8_t r = rgba[0]; // 0 - 255
    uint8_t g = rgba[1]; // 0 - 255
    uint8_t b = rgba[2]; // 0 - 255
    uint8_t a = rgba[3]; // 0: fully transparent, 255: fully opaque

    if (a) {
      std::cout << "Put pixel at (" << x << ", " << y << ") with color " << (uint32_t) r << ", " << (uint32_t) g << ", " << (uint32_t) b << std::endl;
      matrix->drawPixel(x, y, matrix->Color(r,g,b));
    }
}

void display_icon() {
  matrix->fillScreen(0);

  // auto banana = std::string{"image/png;base64,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"};
  std::cout << "Decode base64 string" << std::endl;
  // auto banana = std::string{"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"};
  auto banana = std::string{"iVBORw0KGgoAAAANSUhEUgAAAAgAAAAICAIAAABLbSncAAAAAXNSR0IArs4c6QAAAJtJREFUCB1jZkACc8M9DDhMDz6/giQGYy6KcKg19ATyGGEiUPrHg6A/l985FvxiQZbQVDZjFuZg95La+uUJE1xCU8Vs32ZpZi6Wf+//8kixQ416ejNYSI6Dg43575vvP06/+vbmD8utGeZijlL8KpxArX9ffv9x/tXLm5+9exmYH3/RCtH8/ufBl1+XPv599+Prq98BE9guPzgJALOLPiGN8SoeAAAAAElFTkSuQmCC"};
  auto wolverine = std::string{"iVBORw0KGgoAAAANSUhEUgAAAAgAAAAICAIAAABLbSncAAAAAXNSR0IArs4c6QAAAIJJREFUCB1jFFewN1VlQAOnbzOwvHxwkEHVfvM2Fbicr9cdkKDXrK9Aof+/kyHo/TQnIBcoyASkgKYxss4FMj7Mvi+YtQ/IBbJB4M+7x0ASqAOiHMgFyjEBMYuQLFgBlIBygRJ9zgZfDy8AKgQiIAPIBQoy+rjar6xLRNYBZIc3zQcACGo40ycedMMAAAAASUVORK5CYII="};
  // auto binary = base64::encode(String{banana.c_str()});
    size_t outputLength;
    unsigned char * binary = base64_decode((const unsigned char *)wolverine.c_str(), wolverine.size(), &outputLength);
  // auto binaryLength = decode_base64_length(banana.c_str(), banana.size());
  // unsigned char binary[6];
// unsigned int binary_length = decode_base64(base64, binary);

  std::cout << "Decode PNG" << std::endl;
    pngle_t *pngle = pngle_new();
    pngle_set_draw_callback(pngle, on_draw);
    // Feed data to pngle
    // char buf[1024];
    // int remain = 0;
    // int len;
    // while ((len = read(STDIN_FILENO, buf + remain, sizeof(buf) - remain)) > 0) {
    //     int fed = pngle_feed(pngle, buf, remain + len);
    //     if (fed < 0) errx(1, "%s", pngle_error(pngle));

    //     remain = remain + len - fed;
    //     if (remain > 0) memmove(buf, buf + fed, remain);
    // }
    auto retValue = pngle_feed(pngle, binary, outputLength);
    std::cout << "retValue: " << retValue << std::endl;

    pngle_destroy(pngle);

    matrix->show();
}

void setup()
{
  // Time for serial port to work?
  delay(1000);
  Serial.begin(115200);

  std::cout << "Init on pin: " << pinMatrix << std::endl;
  std::cout << "Matrix size: " << nbColumns << " x " << nbRows << " = " << nbPixel << " pixel" << std::endl;

  FastLED.addLeds<NEOPIXEL, pinMatrix>(leds, nbPixel).setCorrection(TypicalLEDStrip);
  Serial.print("Setup serial: ");
  Serial.println(nbPixel);

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(brightness);
  Serial.println("If the code crashes here, decrease the brightness or turn off the all white display below");
  // Test full bright of all LEDs. If brightness is too high
  // for your current limit (i.e. USB), decrease it.
//#define DISABLE_WHITE
#ifndef DISABLE_WHITE
  matrix->fillScreen(LED_WHITE_HIGH);
  matrix->show();
  delay(3000);
  matrix->clear();
#endif
};



void loop()
{
  // display_runningPixel();
  display_scrollText("Hello World");
  // display_scrollText("foobar2000!");
  // display_scrollText("ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜabcdefghijklmnopqrstuvwxyzäöü1234567890");
  display_icon();
  delay(10000);
};