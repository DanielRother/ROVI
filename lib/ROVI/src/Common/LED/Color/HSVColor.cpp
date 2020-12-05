#include "HSVColor.hpp"

#include <Arduino.h>

#include <string>
#include <stdint.h>
#include <sstream>
#include <algorithm>
#include <assert.h>

#include <EnumUtils.hpp>
#include <ArduinoIostream.hpp>

namespace Rovi {
    HSVColor::HSVColor(float h, float s, float v)
        : h(h), s(s), v(v)
    {
        // Limit range
        while(h > 359) {
            h -= 360.0f;
        }
        while(h < 0) {
            h += 360.0f;
        }
    }

    HSVColor::HSVColor()
        : h(0.0f), s(1.0f), v(1.0f)
    {}

    HSVColor::HSVColor(RGBColor rgb) 
    : h(255), s(1.0f), v(1.0f) {
        float min, max, delta;
        float r = (float) rgb.r / 255.0f;
        float g = (float) rgb.g / 255.0f;
        float b = (float) rgb.b / 255.0f;

        min = r < g ? r : g;
        min = min  < b ? min  : b;

        max = r > g ? r : g;
        max = max  > b ? max  : b;

        v = max;                                // v
        delta = max - min;
        if (delta < 0.00001f)
        {
            s = 0.0f;
            h = 0.0f; // undefined, maybe nan?
            return;
        }
        if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
            s = (delta / max);                  // s
        } else {
            // if max is 0, then r = g = b = 0              
            // s = 0, h is undefined
            s = 0.0f;
            h = std::numeric_limits<float>::signaling_NaN();                            // its now undefined
            return;
        }
        if( r >= max )                           // > is bogus, just keeps compilor happy
            h = ( g - b ) / delta;        // between yellow & magenta
        else
        if( g >= max )
            h = 2.0f + ( b - r ) / delta;  // between cyan & yellow
        else
            h = 4.0f + ( r - g ) / delta;  // between magenta & cyan

        h *= 60.0f;                              // degrees

        if( h < 0.0f ) {
            h += 360.0f;
        }
    }

    HSVColor::HSVColor(const std::shared_ptr<HSVColor>& hsv)
        : h(hsv->h), s(hsv->s), v(hsv->v)
    {}

    HSVColor::HSVColor(const std::shared_ptr<RGBColor>& rgb)
        : HSVColor(RGBColor(rgb))
    {}


    std::string HSVColor::toString() {
        std::stringstream ret;
        ret << "H: " << h << "; S: " << s << "; V: " << v  << ";" << std::endl;
        return ret.str();
    }

    std::shared_ptr<RGBColor> HSVColor::toRGB() {
        return std::make_shared<RGBColor>(*this);
    }

    std::shared_ptr<HSVColor> HSVColor::toHSV() {
        return shared_from_this();
    }
}
