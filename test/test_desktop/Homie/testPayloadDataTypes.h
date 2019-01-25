#include <PayloadDataTypes.h>
#include <AUnit.h>

namespace Rovi {
    namespace Homie {
        test(Homie_PayploadDataTypes, String) {
            // String types are limited to 268,435,456 characters
            // An empty string (“”) is a valid payload
            auto value = String{""};
            assertTrue(value.validateValue(""));
            assertTrue(value.validateValue("abc"));
        }

        test(Homie_PayploadDataTypes, Integer) {
            // Integer types are UTF-8 encoded string literal representations of 64-bit signed whole numbers
            // Integers range from -9,223,372,036,854,775,808 (-263) to 9,223,372,036,854,775,807 (263-1)
            // The payload may only contain whole numbers and the negation character “-”. No other characters including spaces (” “) are permitted
            // A string with just a negation sign (“-”) is not a valid payload
            // An empty string (“”) is not a valid payload
            auto value = Integer{0};

            assertTrue(value.validateValue("123"));
            assertTrue(value.validateValue("-123"));
            assertFalse(value.validateValue("123.456"));
            assertFalse(value.validateValue("abc"));
            assertFalse(value.validateValue("-"));
            assertFalse(value.validateValue(""));
            assertFalse(value.validateValue(" "));

            assertTrue(Integer{"123"} == Integer{123});
            assertTrue(Integer{"-123"} == Integer{-123});
            assertFalse(Integer{"123"} == Integer{-123});
            assertTrue(Integer{"123"} != Integer{456});

            assertTrue("123" == Integer{123}.toString());
            assertTrue("-123" == Integer{-123}.toString());
        }

        test(Homie_PayploadDataTypes, Float) {
            // Float types are UTF-8 encoded string literal representations of 64-bit signed floating point numbers
            // Floats range from 2-1074 to (2-2-52)*21023
            // The payload may only contain whole numbers, the negation character “-”, the exponent character “e” or “E” and the decimal separator “.”, no other characters, including spaces (” “) are permitted
            // The dot character (“.”) is the decimal separator (used if necessary) and may only have a single instance present in the payload
            // Representations of numeric concepts such as “NaN” (Not a Number) and “Infinity” are not a valid payload
            // A string with just a negation sign (“-”) is not a valid payload
            // An empty string (“”) is not a valid payload
            auto value = Float{0};
            assertTrue(value.validateValue("123"));
            assertTrue(value.validateValue("-123"));
            assertTrue(value.validateValue("123.456"));
            assertTrue(value.validateValue("-123.456"));
            assertTrue(value.validateValue(".456"));
            assertTrue(value.validateValue("-.456"));
            assertFalse(value.validateValue("123.456.789"));
            assertFalse(value.validateValue("-123.456.789"));
            assertTrue(value.validateValue("2e8"));
            assertTrue(value.validateValue("2E8"));
            assertFalse(value.validateValue("2f8"));
            assertFalse(value.validateValue("NaN"));
            assertFalse(value.validateValue("Infinity"));
            assertFalse(value.validateValue("-"));
            assertFalse(value.validateValue(""));
            assertFalse(value.validateValue(" "));

            assertTrue(Float{"123"} == Float{123});
            assertTrue(Float{"123.456"} == Float{123.456});
            assertTrue(Float{"-123"} == Float{-123});
            assertTrue(Float{"-123.456"} == Float{-123.456});
            assertTrue(Float{".456"} == Float{.456});
            assertTrue(Float{"-.456"} == Float{-.456});
            assertTrue(Float{"2e8"} == Float{2e8});
            assertTrue(Float{"2E8"} == Float{2E8});
            assertTrue(Float{"-2E8"} == Float{-2E8});
            assertTrue(Float{"123"} != Float{456.789});

            assertTrue("123" == Float{123}.toString());
            assertTrue("123.456" == Float{123.456}.toString());
            assertTrue("-123" == Float{-123}.toString());
            assertTrue("0.456" == Float{.456}.toString());
            assertTrue("-123.456" == Float{-123.456}.toString());
            assertTrue("-0.456" == Float{-.456}.toString());
            assertTrue("2e+08" == Float{2e8}.toString());
            assertTrue("2e+08" == Float{2E8}.toString());
            assertTrue("-2e+08" == Float{-2E8}.toString());
        }

        test(Homie_PayploadDataTypes, Boolean) {
            // Booleans must be converted to the string literals “true” or “false”
            // Representation is case sensitive, e.g. “TRUE” or “FALSE” are not valid payloads.
            // An empty string (“”) is not a valid payload
            auto value = Boolean{false};

            assertTrue(value.validateValue("true"));
            assertTrue(value.validateValue("false"));
            assertFalse(value.validateValue("True"));
            assertFalse(value.validateValue("False"));
            assertFalse(value.validateValue("TRUE"));
            assertFalse(value.validateValue("FALSE"));
            assertFalse(value.validateValue("0"));
            assertFalse(value.validateValue("1"));
            assertFalse(value.validateValue("123"));
            assertFalse(value.validateValue("123.456"));
            assertFalse(value.validateValue("abc"));
            assertFalse(value.validateValue("-"));
            assertFalse(value.validateValue(""));
            assertFalse(value.validateValue(" "));

            assertTrue(Boolean{"true"} == Boolean{true});
            assertTrue(Boolean{"false"} == Boolean{false});
            assertTrue(Boolean{"true"} != Boolean{false});
            assertTrue(Boolean{"false"} != Boolean{true});

            assertTrue("true" == Boolean{true}.toString());
            assertTrue("false" == Boolean{false}.toString());
        }

    }
}