#ifndef __ROVI_RGBCOLOR_H__
#define __ROVI_RGBCOLOR_H__

#include <Arduino.h>

#include <string>
#include <stdint.h>
#include <sstream>

struct colorTuple {
  colorTuple(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
    : R(R), G(G), B(B), A(A)
  {}

  colorTuple(uint8_t R, uint8_t G, uint8_t B)
    : R(R), G(G), B(B), A(255)
  {}

  colorTuple()
      : R(255), G(255), B(255), A(255)
  {}

  
  uint8_t R;
  uint8_t G;
  uint8_t B;
  uint8_t A;

  std::string toString() {
    std::stringstream ret;
    ret << "R: " << (uint32_t) R << "; G: " << (uint32_t) G << "; B: " << (uint32_t) B << "; A: " << (uint32_t) A << ";" << std::endl;
    
    ret << "RGBA(" << (uint32_t) R << "," << (uint32_t) G << "," << (uint32_t) B << "," << (uint32_t) A << ")" << std::endl;
    return ret.str();
  }
};

#endif /* __ROVI_RGBCOLOR_H__ */