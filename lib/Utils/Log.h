#if !defined(ROVI_UTILS_LOH_H)
#define ROVI_UTILS_LOH_H

#include <Arduino.h>
#include <string>

#include "ArduinoIostream.hpp"

// TODO: Implement some real logging

// namespace Rovi {
//     namespace Utils {
        void LOG_FATAL(const std::string& msg) {}
        void LOG_DEBUG(const std::string& msg) {
            // Serial << "DEBUG: " << msg << endl;
        }
//     }
// }

#endif // ROVI_UTILS_LOH_H
