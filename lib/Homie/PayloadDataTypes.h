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
            }
            bool validate(const std::string& value) {
                valid = validateValue(value);
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

        // template<typename T>
        // std::shared_ptr<PayloadDatatype> createPayload(const std::string& payload) {

        // }

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
                valueFromString(payload); // TBD
            }

            virtual bool validateValue(const std::string& value) const override {
                // TODO: Add check
                // Hier macht ein String als Argument mehr Sinn..
                // isValid = payload.size() >= 0 && payload.size() < 268435456;
                return true;
            }

            virtual PayloadDatatype::ValueType valueFromString(const std::string& payload) const override {
                return atoll(payload.c_str());
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
