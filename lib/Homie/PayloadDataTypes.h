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

        class Integer : public PayloadDatatype<uint64_t> {
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
                return to_string(value);
            }       
        };


        class Enumeration : public PayloadDatatype<std::string> {
        public:
            Enumeration(const std::set<std::string>& enumValues) : PayloadDatatype(), enumValues(enumValues) {
            }

            virtual bool validateValue(const std::string& payload) const override {
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
    }
}

#endif /* ROVI_HOMIE_PAYLOAD_DATATYPES_H */
