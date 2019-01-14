#ifndef ROVI_HOMIE_PAYLOAD_DATATYPES_H
#define ROVI_HOMIE_PAYLOAD_DATATYPES_H

#include <stdlib.h>
#include <string>
#include <memory>

#include "FileIOUtils.hpp"

namespace Rovi {
    namespace  Homie {
        template<typename T>
        class PayloadDatatype {
        public:
            T value() const {
                return data;
            }
            bool isValid() const {
                return valid;
            }

            virtual bool validate(const T& value) = 0; // TBD: Datatype of argument
            virtual void fromString(const std::string& payload) = 0;
            virtual std::string toString() const = 0;

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

            virtual bool validate(const std::string& value) override {
                valid = value.size() >= 0 && value.size() < 268435456;
                return valid;
            }

            virtual void fromString(const std::string& payload) override {
                validate(payload);
                data = payload;
            }

            virtual std::string toString() const override {
                return data;
            }       
        };

        class Integer : public PayloadDatatype<std::uint64_t> {
        public:
            Integer(const std::string& payload = "0") : PayloadDatatype() {
                fromString(payload);
            }

            virtual bool validate(const uint64_t& value) override {
                // TODO: Add check
                // isValid = payload.size() >= 0 && payload.size() < 268435456;
                valid = true;
                return valid;
            }

            virtual void fromString(const std::string& payload) override {
                auto value = atoll(payload.c_str());
                validate(value);
                data = value;
            }

            virtual std::string toString() const override {
                return to_string(data);
            }       
        };
    }
}

#endif /* ROVI_HOMIE_PAYLOAD_DATATYPES_H */
