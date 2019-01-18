#ifndef ROVI_HOMIE_PAYLOAD_DATATYPES_H
#define ROVI_HOMIE_PAYLOAD_DATATYPES_H

#include <stdlib.h>
#include <string>
#include <memory>
#include <set>

#include "FileIOUtils.hpp"

namespace Rovi {
    namespace  Homie {
        template<typename T>
        class PayloadDatatype {
        public:
            using ValueType = T;

            virtual bool validateValue(const std::string& value) const = 0;
            virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const = 0;
            virtual std::string valueToString(const PayloadDatatype::ValueType& value) const = 0;

            bool validateT(const T& value) {                    // TODO: typetraits o.ä. für T == string
                valid = validateValue(valueToString(value));
                return valid;
            }
            bool validate(const std::string& value) {
                valid = validateValue(value);
                return valid;
            }
            bool isValid() const {
                return valid;
            }
            ValueType value() const {
                return data;
            }
            std::string toString() const {
                return valueToString(data);
            }
            void setValueT(const ValueType& value) {        // s.o.
                setValue(valueToString(value));
            }
            void setValue(const std::string& value) {
                validate(value);
                setValue(value);
            }



        protected:
            PayloadDatatype() : valid(false) {
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
                setValue(payload);
            }
            Integer(const PayloadDatatype::ValueType& payload = 0) : PayloadDatatype() {
                setValueT(payload);
            }

            virtual bool validateValue(const std::string& value) const override {
                // TODO: Add check
                bool isValid = true;
                // isValid &= rangeCheck();                     // TODO: Is this possible at this point? What is returned by atoll?
                // isValid &= TODO: regex für [0123456789-]*    // The payload may only contain whole numbers and the negation character “-”. No other characters including spaces (” “) are permitted
                isValid &= !(value == "-");                    // A string with just a negation sign (“-”) is not a valid payload
                isValid &= !(value == "");                      // An empty string (“”) is not a valid payload
                return isValid;
            }

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
                setValueT(payload);
            }

            virtual bool validateValue(const std::string& value) const override {
                // TODO: Add check
                bool isValid = true;
                // isValid &= rangeCheck();                     // TODO: Is this possible at this point?
                // isValid &= TODO: regex für [0123456789-eE.]*    // The payload may only contain whole numbers, the negation character “-”, the exponent character “e” or “E” and the decimal separator “.”, no other characters, including spaces (” “) are permitted
                                                                // This included: Representations of numeric concepts such as “NaN” (Not a Number) and “Infinity” are not a valid payload
                // isValid &= count(value, ".") <= 1;                    // TODO: The dot character (“.”) is the decimal separator (used if necessary) and may only have a single instance present in the payload
                isValid &= !(value == "-");                    // A string with just a negation sign (“-”) is not a valid payload
                isValid &= !(value == "");                      // An empty string (“”) is not a valid payload
                return isValid;
            }

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
                setValueT(payload);
            }

            virtual bool validateValue(const std::string& value) const override {
                bool isValid = true;
                isValid &= (value == "true" || value == "false");           // Booleans must be converted to the string literals “true” or “false”
                                                                            // Representation is case sensitive, e.g. “TRUE” or “FALSE” are not valid payloads.
                                                                            // An empty string (“”) is not a valid payload
                return isValid;
            }

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

            virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const override {
                return payload;
            }

            virtual std::string valueToString(const PayloadDatatype::ValueType& value) const override {
                return value;
            }   

        protected:
            std::set<std::string> enumValues; 
        };

        enum class ColorFormat {
            RGB,
            HSV
        };

        // TODO
        class Color : public PayloadDatatype<std::tuple<int64_t, int64_t, int64_t>> {
        public:
            Color(const ColorFormat format, const std::string& payload = "0,0,0") : PayloadDatatype(), format(format) {
                setValue(payload); // TBD
            }
            Color(const ColorFormat format, const PayloadDatatype::ValueType& payload = {0,0,0}) : PayloadDatatype(), format(format) {
                setValueT(payload);
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

                auto convValue = valueFromString(value);
                switch(format) {
                    case ColorFormat::RGB:
                        auto r = std::get<0>(convValue);
                        auto g = std::get<1>(convValue);
                        auto b = std::get<2>(convValue);
                        isValid &= r > 0 && r <= 255;
                        isValid &= g > 0 && g <= 255;
                        isValid &= b > 0 && b <= 255;
                        break;
                    case ColorFormat::HSV:
                        auto h = std::get<0>(convValue);
                        auto s = std::get<1>(convValue);
                        auto v = std::get<2>(convValue);
                        isValid &= h > 0 && h <= 360;
                        isValid &= s > 0 && s <= 100;
                        isValid &= v > 0 && v <= 100;
                        break;
                    default:
                        isValid = false;
                        break;
                }

                return isValid;
            }

            virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const override {
                auto values = splitString(payload, ',');
                return PayloadDatatype::ValueType{values[0], values[1], values[2]};
            }

            virtual std::string valueToString(const PayloadDatatype::ValueType& value) const override {
                // TODO
                std::string payload = "false";
                if(value == true) {
                    payload == "true";
                }
                return payload;
            }     

            protected:
            ColorFormat format;  
        };
    }
}

#endif /* ROVI_HOMIE_PAYLOAD_DATATYPES_H */
