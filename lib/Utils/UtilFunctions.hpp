#ifndef __UTILS_FUNCTION_H__
#define __UTILS_FUNCTION_H__

#include <Arduino.h>
#include <algorithm>

namespace Rovi {
    namespace Utils {
        template<typename T> 
        inline T clamp(T value, T minVal, T maxVal) {
            return std::max(std::min(value, maxVal), minVal);
        }

        // TBD: What abou overloaded methods, like float percent (range 0-1)?
        template<typename T>
        inline T scale(uint8_t percent, T maxValue) {
            assert(percent >= 0 && percent <= 100);
            auto percentFloat = (float) percent / 100.0f;
            return (float) maxValue * percentFloat; 
        }
    }
}

#endif /* __UTILS_FUNCTION_H__ */