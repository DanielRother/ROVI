#ifndef __ROVI_RGBCOLOR_H__
#define __ROVI_RGBCOLOR_H__

#include <Arduino.h>

#include <string>
#include <stdint.h>
#include <sstream>

#include <enumUtils.hpp>

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

#endif /* __ROVI_RGBCOLOR_H__ */