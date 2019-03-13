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
    }
}

#endif /* __UTILS_FUNCTION_H__ */