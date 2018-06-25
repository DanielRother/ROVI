#ifndef __ROVI_RGBCOLOR_H__
#define __ROVI_RGBCOLOR_H__

#include <Arduino.h>

#include <string>
#include <stdint.h>
#include <sstream>

#include <EnumUtils.hpp>

class RGBAColor {
  enum class RGBAIdx {
    R = 0,
    G = 1,
    B = 2,
    A = 3
  };

public:
  RGBAColor(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
    : R(R), G(G), B(B), A(A)
  {}

  RGBAColor(uint8_t R, uint8_t G, uint8_t B)
    : R(R), G(G), B(B), A(255)
  {}

  RGBAColor()
      : R(255), G(255), B(255), A(255)
  {}

  RGBAColor(std::string payload) 
    : R(255), G(255), B(255), A(255)
  {
    // Palyoad format: RGBA(54,255,46,255)

    // 1. First extract the value string, i.e. the numbers
    size_t      valuesStart = payload.find('(') + 1; 
    size_t      valuesEnd   = payload.find(')'); 
    std::string values      = payload.substr(valuesStart, valuesEnd - valuesStart);
    // Serial << "start: " << valuesStart << "\tend: " << valuesEnd << endl << "Values: " << values.c_str() << endl;

    // 2. Extract the actual RGB values
    std::stringstream ss(values);
    std::string token;
    std::vector<int> rgba;
    while (std::getline(ss, token, ',')) {
        int curVal = atoi(token.c_str());
        rgba.push_back(curVal);
    }
    // std::stringstream ss2;
    // ss2 << rgba << std::endl;
    // Serial <<  "rgba new: " << ss2.str().c_str() << endl;

    // 3. Initialize class members
    assert(rgba.size() >= RGB_SIZE && rgba.size() <= RGBA_SIZE);

    this->R = rgba[to_underlying(RGBAIdx::R)];
    this->G = rgba[to_underlying(RGBAIdx::G)];
    this->B = rgba[to_underlying(RGBAIdx::B)];
    if (rgba.size() == RGBA_SIZE) {
      this->A = rgba[to_underlying(RGBAIdx::A)];
    }    
  }


  std::string toString() {
    // Arduino STL doesn't support C++11... :(
    // Therefore, this stupid  convertion is required...  
    std::stringstream ret;
    ret << "R: " << (uint32_t) R << "; G: " << (uint32_t) G << "; B: " << (uint32_t) B << "; A: " << (uint32_t) A << ";" << std::endl;
    return ret.str();

    // // Once C++11 is supported
    // ret = "R: " + std::to_string(R) + "; G: " + std::to_string(G) + "; B: " + std::to_string(B) + "; A: " + std::to_string(A) + ";";
    // return ret;
  }

  static const size_t RGB_SIZE  = 3;
  static const size_t RGBA_SIZE = 4;

// private:
  uint8_t R;
  uint8_t G;
  uint8_t B;
  uint8_t A;
};



// Source: https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}


#endif /* __ROVI_RGBCOLOR_H__ */