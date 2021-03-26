#ifndef __LEDMATRIXCOMPONENT_H__
#define __LEDMATRIXCOMPONENT_H__

#include <Arduino.h>

#include <iostream>
#include <string>
#include <vector>

#include <ArduinoIostream.hpp>

#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>

#include "Common/LED/ColorTypes.h"
#include "LEDComponent.hpp"

namespace Rovi {
namespace Components {
template <uint8_t PIN> class LedMatrix : public AdressableLedComponent {
public:
  LedMatrix(const int32_t nbColumns, const int32_t nbRows,
            const uint8_t matrixType = NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
                                       NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
            const std::string &name = "LedMatrix")
      : AdressableLedComponent{nbColumns * nbRows, name},
        m_nbColumns{nbColumns}, m_nbRows{nbRows},
        m_leds(m_nbColumns * m_nbRows),
        m_matrix(m_leds.data(), nbColumns, nbRows, matrixType),
        m_textColor(std::make_shared<HSVColor>(0, 1.0, 1.0)) {
    std::cout << "LedMatrix::ctor()" << std::endl;
    std::cout << "m_nbColumns = " << m_nbColumns << ", m_nbRows = " << m_nbRows
              << std::endl;
    std::cout << "m_leds.size() = " << m_leds.size() << std::endl;

    const auto todoBrightness = 16;

    FastLED.addLeds<NEOPIXEL, PIN>(m_leds.data(), m_leds.size())
        .setCorrection(TypicalLEDStrip);
    m_matrix.begin();
    m_matrix.setTextWrap(false);
    m_matrix.setBrightness(todoBrightness);
    std::cout << "If the code crashes here, decrease the brightness or turn "
                 "off the all white display below"
              << std::endl;
    // Test full bright of all LEDs. If brightness is too high
    // for your current limit (i.e. USB), decrease it.
//#define DISABLE_WHITE
#ifndef DISABLE_WHITE
    m_matrix.fillScreen(
        convertColor(std::make_shared<RGBColor>(255, 255, 255)));
    m_matrix.show();
    delay(3000);
    m_matrix.clear();
#endif
  };

  LedMatrix(const LedMatrix &other) = default;
  virtual ~LedMatrix() = default;

  virtual void setColorImpl(const std::shared_ptr<Color> &color) override {
    // Serial << "--- LedMatrix::setColor(): " << color->toString() <<
    // endl;

    auto rgb = color->toRGB();
    for (uint16_t pixelIdx = 0; pixelIdx < m_leds.size(); ++pixelIdx) {
      if (m_swapRGValues[pixelIdx] == 0) {
        m_leds[pixelIdx] = CRGB(rgb->r, rgb->g, rgb->b);
      } else {
        m_leds[pixelIdx] = CRGB(rgb->g, rgb->r, rgb->b);
      }
    }
    m_matrix.show();
    m_color = color;
  }

  virtual void setColorImpl(const std::shared_ptr<Color> &color,
                            size_t pixelIdx) override {
    // Serial << "--- LedMatrix::setColor" << endl;
    AdressableLedComponent::setColorImpl(color, pixelIdx);

    auto rgb = color->toRGB();
    if (m_swapRGValues[pixelIdx] == 0) {
      m_leds[pixelIdx] = CRGB(rgb->r, rgb->g, rgb->b);
    } else {
      m_leds[pixelIdx] = CRGB(rgb->g, rgb->r, rgb->b);
    }
    m_matrix.show();
    m_color = color;
  }

  virtual void setBrightnessImpl(uint8_t brightness) override {
    Serial << "--- LedMatrix::setBrightness to " << (uint32_t)brightness
           << endl;
    m_matrix.setBrightness(brightness);
    // for(uint16_t pixelIdx = 0; pixelIdx < NB_PIXEL; ++pixelIdx) {
    //     // m_leds[pixelIdx].fadeToBlackBy( brightness );
    // }
    m_matrix.show();
  }

  virtual void showImpl() override { m_matrix.show(); }

  void setTextColor(const std::shared_ptr<Color> &color) {
    m_textColor = color;
    m_matrix.setTextColor(convertColor(m_textColor));
  }

  // TODO: Make the effects async (like the regular led effects?)
  void display_scrollText(const std::string &text) {
    std::cout << "display scrollText " << text << std::endl;
    const auto xLimit = -1 * ((int)text.size() * CHAR_SIZE + m_matrix.width());
    std::cout << "xLimit = " << xLimit << std::endl;

    for (auto x = m_matrix.width(); x > xLimit; --x) {
      m_matrix.fillScreen(0);
      m_matrix.setCursor(x, 0);
      m_matrix.print(F(text.c_str()));
      m_matrix.show();
      delay(100);
    }
  }

protected:
  uint16_t convertColor(std::shared_ptr<Color> color) {
    auto rgbColor = color->toRGB();
    return m_matrix.Color(rgbColor->r, rgbColor->g, rgbColor->b);
  }

  int32_t m_nbColumns;
  int32_t m_nbRows;
  std::vector<CRGB> m_leds;
  FastLED_NeoMatrix m_matrix;

  std::shared_ptr<Color> m_textColor;

  static const auto CHAR_SIZE = 6;
};
} // namespace Components
} // namespace Rovi

#endif /* __LEDMATRIXCOMPONENT_H__ */