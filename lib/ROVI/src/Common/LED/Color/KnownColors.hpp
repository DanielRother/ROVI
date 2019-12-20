#ifndef __ROVI_KNOWNCOLORS_H__
#define __ROVI_KNOWNCOLORS_H__

#include "Color.hpp"
#include "RGBColor.hpp"
#include "HSVColor.hpp"

namespace Rovi {
    static const std::shared_ptr<RGBColor> tuYellow = std::make_shared<RGBColor>(255,200,42);
    static const std::shared_ptr<RGBColor> tuOrange = std::make_shared<RGBColor>(225,109,0);
    static const std::shared_ptr<RGBColor> tuRed = std::make_shared<RGBColor>(113,28,47);

    static const std::shared_ptr<RGBColor> tuLightBlue = std::make_shared<RGBColor>(102,180,211);
    static const std::shared_ptr<RGBColor> tuBlue = std::make_shared<RGBColor>(0,112,155);
    static const std::shared_ptr<RGBColor> tuDarkBlue = std::make_shared<RGBColor>(0,63,87);

    static const std::shared_ptr<RGBColor> tuLightGreen = std::make_shared<RGBColor>(172, 193, 58);
    static const std::shared_ptr<RGBColor> tuGreen = std::make_shared<RGBColor>(109, 131, 0);
    static const std::shared_ptr<RGBColor> tuDarkGreen = std::make_shared<RGBColor>(0, 83, 74);

    static const std::shared_ptr<RGBColor> tuLightPurple = std::make_shared<RGBColor>(138, 48, 127);
    static const std::shared_ptr<RGBColor> tuPurple = std::make_shared<RGBColor>(81, 18, 70);
    static const std::shared_ptr<RGBColor> tuDarkPurple = std::make_shared<RGBColor>(76, 24, 48);

}

#endif