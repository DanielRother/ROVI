#include "Color.hpp"

#include <Arduino.h>

#include <string>
#include <stdint.h>
#include <sstream>
#include <algorithm>
#include <assert.h>

#include <EnumUtils.hpp>
#include <ArduinoIostream.hpp>

#include "RGBColor.hpp"
#include "HSVColor.hpp"

std::shared_ptr<Color> Color::createColor(std::string payload) {
    // Palyoad format: RGB(54,255,46) or HSV(128, 0.5, 0.7)

    // 0. Determine actual color type
    assert(payload.size() >= (3+2+2+3));    // i.e. 'RGB, '()', ',' 'values'
    ColorType colorType = ColorType::UNKNOWN; 
    std::string colorTypeString = payload.substr(0,3);
    std::transform(colorTypeString.begin(), colorTypeString.end(),colorTypeString.begin(), ::toupper);    // Convert to upper
    if(colorTypeString == "RGB") {
        colorType = ColorType::RGB;
        Serial << "  RGBColorType" << endl;
    } else if (colorTypeString == "HSV"){
        colorType = ColorType::HSV;
        Serial << "  HSVColorType" << endl;
    }  else {
        Serial << "  ERROR: Unknown ColorType. Returning" << endl;
        return nullptr;
    }

    // 1. First extract the value string, i.e. the numbers
    size_t      valuesStart = payload.find('(') + 1; 
    size_t      valuesEnd   = payload.find(')'); 
    std::string values      = payload.substr(valuesStart, valuesEnd - valuesStart);
    // Serial << "start: " << valuesStart << "\tend: " << valuesEnd << endl << "Values: " << values.c_str() << endl;

    // 2. Extract the actual RGB values
    std::stringstream ss(values);
    std::string token;
    std::vector<float> colorValues;
    while (std::getline(ss, token, ',')) {
        float curVal = atof(token.c_str());
        colorValues.push_back(curVal);
    }
    // std::stringstream ss2;
    // ss2 << colorValues << std::endl;
    // Serial <<  "colorValues new: " << ss2.str().c_str() << endl;

    // 3. Initialize class members
    assert(colorValues.size() == 3);

    float r,g,b,h,s,v;
    r = h = colorValues[0];
    g = s = colorValues[1];
    b = v = colorValues[2];

    std::shared_ptr<Color> color;
    switch (colorType) {
        case ColorType::RGB:
            assert(r >= 0.0f && r <= 255.0f);
            assert(g >= 0.0f && g <= 255.0f);
            assert(b >= 0.0f && b <= 255.0f);
            if(r < 0.0f || r > 255.0f || g < 0.0f || g > 255.0f || b < 0.0f || b > 255.0f) {
                Serial << "Warning: RGB values outside range" << endl;
            }
            
            color = std::static_pointer_cast<Color>(std::make_shared<RGBColor>(r, g, b));
            break;

        case ColorType::HSV:
            assert(h >= 0.0f && h <= 360.0f);
            assert(s >= 0.0f && s <= 1.0f);
            assert(v >= 0.0f && v <= 1.0f);
            if(h < 0.0f || h > 360.0f || s < 0.0f || s > 1.0f || v < 0.0f || v > 1.0f) {
                Serial << "Warning: HSV values outside range" << endl;
            }
            
            color = std::static_pointer_cast<Color>(std::make_shared<HSVColor>(h, s, v));
            break;
        default:
        break;
    }

    return color;
}