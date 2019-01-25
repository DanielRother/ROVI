#ifndef ROVI_HOMIE_PAYLOAD_DATATYPES_H
#define ROVI_HOMIE_PAYLOAD_DATATYPES_H

#include <stdlib.h>
#include <string>
#include <memory>
#include <set>
#include <algorithm>
#include <type_traits>

#include "FileIOUtils.hpp"
#include "Log.h"

namespace Rovi {
    namespace  Homie {
        template<typename T>
        class PayloadDatatype {
        public:
            using ValueType = T;

            // TODO:
            // Making this function static would allow for a more intuitive interface
            // which could be achieved using temnplates.
            // Unfortunately, the Enum and Color types require access to some member variables
            // making the implenentation NOT straight forward.... :()
            virtual bool validateValue(const std::string& value) const = 0;

            bool isValid() const {
                return valid;
            }
            ValueType value() const {
                return data;
            }
            std::string toString() const {
                return valueToString(data);
            }

            // Only activate if T != std::string. Otherwise there would be two time the same method signbature
            template <typename = std::enable_if<std::is_same<T, std::string>::value == false>>
            void setValue(const ValueType& value) {
                auto convValue = valueToString(value);
                setValue(convValue);
            }
            void setValue(const std::string& value) {
                validate(value);
                data = valueFromString(value);
            }
            bool operator==(const PayloadDatatype<T>& rhs) const {
                return value() == rhs.value();
            }

        protected:
            PayloadDatatype() : valid(false) {
            }

            virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const = 0;
            virtual std::string valueToString(const PayloadDatatype::ValueType& value) const = 0;

            template <typename = std::enable_if<std::is_same<T, std::string>::value == false>>
            bool validate(const T& value) {
                valid = validateValue(valueToString(value));
                return valid;
            }
            bool validate(const std::string& value) {
                valid = validateValue(value);
                return valid;
            }

            ValueType data;
            bool valid;
        };

        class String : public PayloadDatatype<std::string> {
        public:
            String(const std::string& payload = "") : PayloadDatatype() {
                setValue(payload);
            }

            virtual bool validateValue(const std::string& value) const override {
                return value.size() >= 0 && value.size() <= 268435456;;
            }

        protected:
            virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const override {
                return payload;
            }

            virtual std::string valueToString(const PayloadDatatype::ValueType& value) const override {
                return value;
            }     
        };

        class Integer : public PayloadDatatype<int64_t> {
        public:
            Integer(const std::string& payload = "0") : PayloadDatatype() {
                LOG_DEBUG("Integer::ctor(string)");
                setValue(payload);
            }
            Integer(const PayloadDatatype::ValueType& payload = 0) : PayloadDatatype() {
                LOG_DEBUG("Integer::ctor(int)");
                setValue(payload);
            }

            virtual bool validateValue(const std::string& value) const override {
                // TODO: Add check
                bool isValid = true;
                // isValid &= rangeCheck();                     // TODO: Is this possible at this point? What is returned by atoll?
                isValid &= checkStringForAllowedCharacters(value, std::string("01234567890-"));    // The payload may only contain whole numbers and the negation character “-”. No other characters including spaces (” “) are permitted
                isValid &= !(value == "-");                    // A string with just a negation sign (“-”) is not a valid payload
                isValid &= !(value == "");                      // An empty string (“”) is not a valid payload
                return isValid;
            }

         protected:
           virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const override {
                return atoll(payload.c_str());
            }

            virtual std::string valueToString(const PayloadDatatype::ValueType& value) const override {
                return to_string(value);
            }       
        };

        class Float : public PayloadDatatype<double> {
        public:
            Float(const std::string& payload = "0") : PayloadDatatype() {
                setValue(payload); // TBD
            }
            Float(const PayloadDatatype::ValueType& payload = 0.0) : PayloadDatatype() {
                setValue(payload);
            }

            virtual bool validateValue(const std::string& value) const override {
                // TODO: Add check
                bool isValid = true;
                // isValid &= rangeCheck();                     // TODO: Is this possible at this point?
                isValid &= checkStringForAllowedCharacters(value, std::string("01234567890-eE."));
                isValid &= std::count(value.begin(), value.end(), '.') <= 1;                    // The dot character (“.”) is the decimal separator (used if necessary) and may only have a single instance present in the payload
                isValid &= !(value == "-");                    // A string with just a negation sign (“-”) is not a valid payload
                isValid &= !(value == "");                      // An empty string (“”) is not a valid payload
                return isValid;
            }

        protected:
            virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const override {
                return atof(payload.c_str());       // TBD: Richtig?
            }

            virtual std::string valueToString(const PayloadDatatype::ValueType& value) const override {
                return to_string(value);        // TBD: Welche darstellung nutzt der? 123.456 oder 123e456?
            }       
        };

        class Boolean : public PayloadDatatype<bool> {
        public:
            Boolean(const std::string& payload = "false") : PayloadDatatype() {
                setValue(payload); // TBD
            }
            Boolean(const PayloadDatatype::ValueType& payload = false) : PayloadDatatype() {
                setValue(payload);
            }

            virtual bool validateValue(const std::string& value) const override {
                bool isValid = true;
                isValid &= (value == "true" || value == "false");           // Booleans must be converted to the string literals “true” or “false”
                                                                            // Representation is case sensitive, e.g. “TRUE” or “FALSE” are not valid payloads.
                                                                            // An empty string (“”) is not a valid payload
                return isValid;
            }

         protected:
           virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const override {
                bool value = false;
                if(payload == "true") {
                    value = true;
                }
                return value;
            }

            virtual std::string valueToString(const PayloadDatatype::ValueType& value) const override {
                std::string payload = "false";
                if(value == true) {
                    payload == "true";
                }
                return payload;
            }       
        };

        class Enumeration : public PayloadDatatype<std::string> {
        public:
            Enumeration(const std::set<std::string>& enumValues) : PayloadDatatype(), enumValues(enumValues) {
            }

            virtual bool validateValue(const std::string& payload) const override {
                // Enum payloads must be one of the values specified in the format definition of the property
                // Enum payloads are case sensitive, e.g. “Car” will not match a format definition of “car”
                // Payloads should have leading and trailing whitespace removed
                // An empty string (“”) is not a valid payload
                return payload.size() > 0 && enumValues.find(payload) != enumValues.end();
            }

        protected:
            virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const override {
                return payload;
            }

            virtual std::string valueToString(const PayloadDatatype::ValueType& value) const override {
                return value;
            }   

            std::set<std::string> enumValues; 
        };

        enum class ColorFormat {
            RGB,
            HSV
        };

        class Color : public PayloadDatatype<std::tuple<int64_t, int64_t, int64_t>> {
        public:
            Color(const ColorFormat format, const std::string& payload = "0,0,0") : PayloadDatatype(), m_format(format) {
                setValue(payload); // TBD
            }
            Color(const ColorFormat format, const PayloadDatatype::ValueType& payload) : PayloadDatatype(), m_format(format) {
                setValue(payload);
            }

            virtual bool validateValue(const std::string& value) const override {
                // Color payload validity varies depending on the property format definition of either “rgb” or “hsv”
                // Both payload types contain comma separated whole numbers of differing restricted ranges
                // The encoded string may only contain whole numbers and the comma character “,”, no other characters are permitted, including spaces (” “)
                // Payloads for type “rgb” contains 3 comma separated values of numbers with a valid range between 0 and 255. e.g. 100,100,100
                // Payloads for type “hsv” contains 3 comma separated values of numbers. The first number has a range of 0 to 360, the second and third numbers have a range of 0 to 100. e.g. 300,50,75
                // An empty string (“”) is not a valid payload
                bool isValid = true;
                isValid &= (value.size() > 0 && value.size() <= 11);  // max "100,100,100" -> 11 chars
                isValid &= checkStringForAllowedCharacters(value, std::string("01234567890,"));
                
                // Return if string already is invalid. Otherwise, convertion will fail...
                if(!isValid) {
                    return isValid;
                }
                auto convValue = valueFromString(value);
                switch(m_format) {
                    case ColorFormat::RGB: {
                        auto r = std::get<0>(convValue);
                        auto g = std::get<1>(convValue);
                        auto b = std::get<2>(convValue);
                        isValid &= r > 0 && r <= 255;
                        isValid &= g > 0 && g <= 255;
                        isValid &= b > 0 && b <= 255; }
                        break;
                    case ColorFormat::HSV: {
                        auto h = std::get<0>(convValue);
                        auto s = std::get<1>(convValue);
                        auto v = std::get<2>(convValue);
                        isValid &= h > 0 && h <= 360;
                        isValid &= s > 0 && s <= 100;
                        isValid &= v > 0 && v <= 100; }
                        break;
                }

                return isValid;
            }

        protected:
           virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const override {
                auto values = splitString(payload, ',');
                return PayloadDatatype::ValueType{atoll(values[0].c_str()), atoll(values[1].c_str()), atoll(values[2].c_str())};
            }

            virtual std::string valueToString(const PayloadDatatype::ValueType& value) const override {
                return to_string(std::get<0>(value)) + "," + to_string(std::get<1>(value)) + "," + to_string(std::get<2>(value));
            }    

            ColorFormat format() const {
                return m_format;
            } 

            void setFormat(const ColorFormat format) {
                m_format = format;
            }

                ColorFormat m_format;  
        };
    }
}

#endif /* ROVI_HOMIE_PAYLOAD_DATATYPES_H */
