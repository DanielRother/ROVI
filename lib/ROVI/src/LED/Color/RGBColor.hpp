#ifndef __RGBCOLOR_H__
#define __RGBCOLOR_H__

#include "Color.hpp"
#include "HSVColor.hpp"

class RGBColor : public Color, public std::enable_shared_from_this<RGBColor> {
public:
  enum class RGBIdx {
    R = 0,
    G = 1,
    B = 2,
  };

  RGBColor();
  RGBColor(uint8_t r, uint8_t g, uint8_t b);
  RGBColor(HSVColor hsv);

  virtual std::string toString() override;
  virtual std::shared_ptr<RGBColor> toRGB() override;
  virtual std::shared_ptr<HSVColor> toHSV() override;

  static const size_t RGB_SIZE  = 3;

// protected:
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

#endif /* __RGBCOLOR_H__ */