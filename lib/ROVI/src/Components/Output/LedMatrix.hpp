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

namespace Rovi {
namespace Components {
template <uint8_t PIN> class LedMatrix {
public:
  LedMatrix(const int32_t nbColumns, const int32_t nbRows,
            const uint8_t matrixType = NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
                                       NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
            const std::string &name = "LedMatrix")
      : m_nbColumns{nbColumns}, m_nbRows{nbRows},
        m_leds(m_nbColumns * m_nbRows),
        m_matrix(m_leds.data(), nbColumns, nbRows, matrixType),
        m_color(std::make_shared<HSVColor>(0, 1.0, 0.5)) {
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

  // TODO: Make the effects async (like the regular led effects?)
  void display_scrollText(const std::string &text) {
    std::cout << "display scrollText " << text << std::endl;
    const auto xLimit = -1 * ((int)text.size() * CHAR_SIZE + m_matrix.width());
    std::cout << "xLimit = " << xLimit << std::endl;

    m_matrix.setTextColor(convertColor(m_color));
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

  std::shared_ptr<Color> m_color;

  static const auto CHAR_SIZE = 6;
};
} // namespace Components
} // namespace Rovi

#endif /* __LEDMATRIXCOMPONENT_H__ */