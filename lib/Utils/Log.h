#if !defined(ROVI_UTILS_LOH_H)
#define ROVI_UTILS_LOH_H

#include <Arduino.h>
#include <string>

#include "ArduinoIostream.hpp"

namespace Rovi {
    namespace Utils {
        void LOG_FATAL(const std::string& msg) {}
    }
}

#endif // ROVI_UTILS_LOH_H
