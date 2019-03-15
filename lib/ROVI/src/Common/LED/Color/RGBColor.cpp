#include "RGBColor.hpp"

#include <Arduino.h>

#include <string>
#include <stdint.h>
#include <sstream>
#include <algorithm>
#include <assert.h>

#include <EnumUtils.hpp>
#include <ArduinoIostream.hpp>

namespace Rovi {
    RGBColor::RGBColor(uint8_t r, uint8_t g, uint8_t b)
        : r(r), g(g), b(b)
    {}

    RGBColor::RGBColor()
        : r(255), g(255), b(255)
    {}

    RGBColor::RGBColor(HSVColor hsv) 
        : r(255), g(255), b(255) {

        float       hh, p, q, t, ff;
        long        i;
        float       tmpR, tmpG, tmpB;     // Required for range convertion

        if(hsv.s <= 0.0f) {       // < is bogus, just shuts up warnings
            r = (hsv.v * 255.0f);
            g = (hsv.v * 255.0f);
            b = (hsv.v * 255.0f);
            return;
        }
        hh = hsv.h;
        if(hh >= 360.0f) hh = 0.0f;
        hh /= 60.0f;
        i = (long)hh;
        ff = hh - i;
        p = hsv.v * (1.0f - hsv.s);
        q = hsv.v * (1.0f - (hsv.s * ff));
        t = hsv.v * (1.0f - (hsv.s * (1.0f - ff)));

        switch(i) {
        case 0:
            tmpR = hsv.v;
            tmpG = t;
            tmpB = p;
            break;
        case 1:
            tmpR = q;
            tmpG = hsv.v;
            tmpB = p;
            break;
        case 2:
            tmpR = p;
            tmpG = hsv.v;
            tmpB = t;
            break;
        case 3:
            tmpR = p;
            tmpG = q;
            tmpB = hsv.v;
            break;
        case 4:
            tmpR = t;
            tmpG = p;
            tmpB = hsv.v;
            break;
        case 5:
        default:
            tmpR = hsv.v;
            tmpG = p;
            tmpB = q;
            break;
        }

        r = (tmpR * 255.0f);
        g = (tmpG * 255.0f);
        b = (tmpB * 255.0f);
    }



    std::string RGBColor::toString() {
        // Arduino STL doesn't support C++11... :(
        // Therefore, this stupid  convertion is required...  
        std::stringstream ret;
        ret << "R: " << (uint32_t) r << "; G: " << (uint32_t) g << "; B: " << (uint32_t) b  << ";" << std::endl;
        return ret.str();

        // // Once C++11 is supported
        // ret = "R: " + std::to_string(R) + "; G: " + std::to_string(G) + "; B: " + std::to_string(B) + "; A: " + std::to_string(A) + ";";
        // return ret;
    }

    std::shared_ptr<RGBColor> RGBColor::toRGB() {
        return shared_from_this();
    }

    std::shared_ptr<HSVColor> RGBColor::toHSV() {
        return std::make_shared<HSVColor>(*this);
    }
}