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
            virtual bool validateValue(const std::string& value) const = 0;
            virtual T fromString(const std::string& payload) const = 0;
            virtual std::string toString(const T& value) const = 0;

            bool validateT(const T& value) {                    // TODO: typetraits o.ä. für T == string
                valid = validateValue(valueString(value);
            }
            bool validate(const std::string& value) {
                valid = validateValue(value);
            }
            bool isValid() const {
                return valid;
            }
            T value() const {
                return data;
            }
            std::string valueString() const {
                return toString(data);
            }
            void setValue(const T& newValue) {
                setValue(toString(newValue));
            }
            void setValue(const std::string& newValue) {
                validate(valueString);
                setValue(valueString);
            }



        protected:
            PayloadDatatype() : valid(false) {
            }

            T data;
            bool valid;
        };

        // template<typename T>
        // std::shared_ptr<PayloadDatatype> createPayload(const std::string& payload) {

        // }

        class String : public PayloadDatatype<std::string> {
        public:
            String(const std::string& payload = "") : PayloadDatatype() {
                fromString(payload);
            }

            virtual bool validateValue(const std::string& value) const override {
                return value.size() >= 0 && value.size() <= 268435456;;
            }

            virtual void fromString(const std::string& payload) const override {
                validateValue(payload);
                data = payload;
            }

            virtual std::string toString(const std::string& value) const override {
                return value;
            }       
        };

        class Integer : public PayloadDatatype<uint64_t> {
        public:
            Integer(const std::string& payload = "0") : PayloadDatatype() {
                fromString(payload);
            }

            virtual bool validateValue(const std::string& value) override {
                // TODO: Add check
                // Hier macht ein String als Argument mehr Sinn..
                // isValid = payload.size() >= 0 && payload.size() < 268435456;
                valid = true;
                return valid;
            }

            virtual void fromString(const std::string& payload) override {
                validateValue(payload);
                auto value = atoll(payload.c_str());
                data = value;
            }

            virtual std::string toString(const uint64_t& value) const override {
                return to_string(value);
            }       
        };

        class Enumeration : public PayloadDatatype<std::string> {
        public:
            Enumeration(const std::set<std::string>& enumValues) : PayloadDatatype(), enumValues(enumValues) {
            }

            virtual bool validateValue(const std::string& payload) override {
                valid = payload.size() > 0 && enumValues.find(payload) != enumValues.end();
                valid = true;
                return valid;
            }

            virtual void fromString(const std::string& payload) override {
                validateValue(payload);
                data = payload;
            }

            virtual std::string toString(const std::string& value) const override {
                return value;
            }   

        protected:
            std::set<std::string> enumValues; 
        };
    }
}

#endif /* ROVI_HOMIE_PAYLOAD_DATATYPES_H */
