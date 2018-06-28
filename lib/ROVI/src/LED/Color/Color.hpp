#ifndef __COLOR_H__
#define __COLOR_H__

#include <Arduino.h>

#include <memory>

enum class ColorType {
  RGB = 0,
  HSV = 1,

  UNKNOWN = 999
};

class RGBColor;
class HSVColor;

class Color {
protected:
  Color() = default;

public:
  static std::shared_ptr<Color> createColor(std::string payload);

  virtual std::string toString() = 0;
  virtual std::shared_ptr<RGBColor> toRGB() = 0;
  virtual std::shared_ptr<HSVColor> toHSV() = 0;
};

#endif /* __COLOR_H__ */