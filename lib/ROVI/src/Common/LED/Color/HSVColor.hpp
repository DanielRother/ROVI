#ifndef __HSVCOLOR_H__
#define __HSVCOLOR_H__

#include "Color.hpp"
#include "RGBColor.hpp"

namespace Rovi {
    class HSVColor : public Color, public std::enable_shared_from_this<HSVColor> {
    public:
      enum class HSVIdx {
        H = 0,
        S = 1,
        V = 2,
      };

      HSVColor();
      HSVColor(float h, float s, float v);
      HSVColor(RGBColor rgb);
      HSVColor(std::string payload);

      virtual std::string toString() override;
      virtual std::shared_ptr<RGBColor> toRGB() override;
      virtual std::shared_ptr<HSVColor> toHSV() override;

      static const size_t HSV_SIZE  = 3;

      // TBD: Define some colors, i.e. red, or at least its h value

    // protected:
      float h;
      float s;
      float v;
    };
}

#endif /* __HSVCOLOR_H__ */