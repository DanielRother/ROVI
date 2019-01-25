#include <PayloadDataTypes.h>
#include <AUnit.h>

// TODO: setValue einbauen

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

            auto successful = value.setValue("123");
            assertTrue(successful);
            assertTrue(value.value() == 123);
            assertTrue(value.toString() == "123");
            assertTrue(value.isValid());

            successful = value.setValue("123.456");
            assertFalse(successful);
            assertTrue(value.value() == 123);
            assertTrue(value.toString() == "123");
            assertTrue(value.isValid());

            successful = value.setValue("567");
            assertTrue(successful);
            assertTrue(value.value() == 567);
            assertTrue(value.toString() == "567");
            assertTrue(value.isValid());
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
            assertTrue(Float{"2E8"} == Float{2E+8});
            assertTrue(Float{"2E8"} == Float{2E+008});
            assertTrue(Float{"-2E8"} == Float{-2E8});
            assertTrue(Float{"2E-8"} == Float{2E-8});
            assertTrue(Float{"2E-8"} == Float{2E-008});
            assertTrue(Float{"-2E-8"} == Float{-2E-8});
            assertTrue(Float{"123"} != Float{456.789});

            assertTrue("123" == Float{123}.toString());
            assertTrue("123.456" == Float{123.456}.toString());
            assertTrue("-123" == Float{-123}.toString());
            assertTrue("0.456" == Float{.456}.toString());
            assertTrue("-123.456" == Float{-123.456}.toString());
            assertTrue("-0.456" == Float{-.456}.toString());
            assertTrue("2e08" == Float{2E8}.toString());
            assertTrue("2e08" == Float{2E+8}.toString());
            assertTrue("2e08" == Float{2E+008}.toString());
            assertTrue("-2e08" == Float{-2E8}.toString());
            assertTrue("2e-08" == Float{2E-8}.toString());
            assertTrue("2e-08" == Float{2E-008}.toString());
            assertTrue("-2e-08" == Float{-2E-8}.toString());

            auto successful = value.setValue("123");
            assertTrue(successful);
            assertTrue(value.value() == 123.0f);
            assertTrue(value.toString() == "123");
            assertTrue(value.isValid());

            successful = value.setValue("123.456.789");
            assertFalse(successful);
            assertTrue(value.value() == 123.0f);
            assertTrue(value.toString() == "123");
            assertTrue(value.isValid());

            successful = value.setValue(".567");
            assertTrue(successful);
            assertTrue(value.value() - .567f < 0.001f);
            assertTrue(value.toString() == "0.567");
            assertTrue(value.isValid());
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

            auto successful = value.setValue("true");
            assertTrue(successful);
            assertTrue(value.value() == true);
            assertTrue(value.toString() == "true");
            assertTrue(value.isValid());

            successful = value.setValue("FALSE");
            assertFalse(successful);
            assertTrue(value.value() == true);
            assertTrue(value.toString() == "true");
            assertTrue(value.isValid());

            successful = value.setValue("false");
            assertTrue(successful);
            assertTrue(value.value() == false);
            assertTrue(value.toString() == "false");
            assertTrue(value.isValid());
        }

        test(Homie_PayploadDataTypes, Enumeration) {
            // Enum payloads must be one of the values specified in the format definition of the property
            // Enum payloads are case sensitive, e.g. “Car” will not match a format definition of “car”
            // Payloads should have leading and trailing whitespace removed
            // An empty string (“”) is not a valid payload
            auto value = Enumeration{{"Red", "Green", "Blue", " White ", "blue and green"}};

            assertTrue(value.validateValue("Red"));
            assertTrue(value.validateValue("Green"));
            assertTrue(value.validateValue("Blue"));
            assertTrue(value.validateValue("White"));
            assertTrue(value.validateValue("blue and green"));
            assertFalse(value.validateValue("red"));
            assertFalse(value.validateValue("RED"));
            assertFalse(value.validateValue("black"));
            assertFalse(value.validateValue(""));
            assertFalse(value.validateValue(" "));

            assertTrue(value.value() == "");
            assertFalse(value.isValid());

            auto successful = value.setValue("Red");
            assertTrue(successful);
            assertTrue(value.value() == "Red");
            assertTrue(value.toString() == "Red");
            assertTrue(value.isValid());
           
            successful = value.setValue("black");
            assertFalse(successful);
            assertTrue(value.value() == "Red");
            assertTrue(value.toString() == "Red");
            assertTrue(value.isValid());

            successful = value.setValue("blue and green");
            assertTrue(successful);
            assertTrue(value.value() == "blue and green");
            assertTrue(value.toString() == "blue and green");
            assertTrue(value.isValid());
        }

        test(Homie_PayploadDataTypes, Color_RGB) {
            // Color payload validity varies depending on the property format definition of either “rgb” or “hsv”
            // Both payload types contain comma separated whole numbers of differing restricted ranges
            // The encoded string may only contain whole numbers and the comma character “,”, no other characters are permitted, including spaces (” “)
            // Payloads for type “rgb” contains 3 comma separated values of numbers with a valid range between 0 and 255. e.g. 100,100,100
            // Payloads for type “hsv” contains 3 comma separated values of numbers. The first number has a range of 0 to 360, the second and third numbers have a range of 0 to 100. e.g. 300,50,75
            // An empty string (“”) is not a valid payload
            auto value = Color{ColorFormat::RGB};
            assertTrue(value.validateValue("0,0,0"));
            assertTrue(value.validateValue("1,1,1"));
            assertTrue(value.validateValue("255,255,255"));
            assertFalse(value.validateValue(""));
            assertFalse(value.validateValue("0, 0, 0"));
            // assertFalse(value.validateValue("0,,0"));        // <- Is handled as 0,0,0. Not sure if I should really dismiss this value
                                                                // Next test case to ensure this behaviour
            auto a = Color{ColorFormat::RGB, "1,,3"};
            auto b = Color{ColorFormat::RGB, ColorTuple{1,0,3}};
            assertTrue(a == b);
            assertFalse(value.validateValue("0,0,0,"));
            assertFalse(value.validateValue("0.0.0"));
            assertFalse(value.validateValue("0.2,0.3,4.5"));
            assertFalse(value.validateValue("-1,0,0"));
            assertFalse(value.validateValue("0,-1,0"));
            assertFalse(value.validateValue("0,0,-1"));
            assertFalse(value.validateValue("256,0,0"));
            assertFalse(value.validateValue("0,256,0"));
            assertFalse(value.validateValue("0,0,256"));

            auto c = Color{ColorFormat::RGB, ColorTuple(4,5,6)};
            assertTrue(a != c);

            {
                auto d = Color{ColorFormat::RGB, ColorTuple(1,2,3)};
                assertTrue("1,2,3" == d.toString());
            }
            {
                auto d = Color{ColorFormat::RGB, "1,,4"};
                assertTrue("1,0,4" == d.toString());
            }

            {   
                auto successful = value.setValue("67,78,112");
                assertTrue(successful);
                assertTrue(value.value() == ColorTuple(67,78,112));
                assertTrue(value.toString() == "67,78,112");
                assertTrue(value.isValid());

                successful = value.setValue("300,78,112");
                assertFalse(successful);
                assertTrue(value.value() == ColorTuple(67,78,112));
                assertTrue(value.toString() == "67,78,112");
                assertTrue(value.isValid());

                successful = value.setValue("67,100,112");
                assertTrue(successful);
                assertTrue(value.value() == ColorTuple(67,100,112));
                assertTrue(value.toString() == "67,100,112");
                assertTrue(value.isValid());
            }
        }
    
        test(Homie_PayploadDataTypes, Color_HSV) {
            // Color payload validity varies depending on the property format definition of either “rgb” or “hsv”
            // Both payload types contain comma separated whole numbers of differing restricted ranges
            // The encoded string may only contain whole numbers and the comma character “,”, no other characters are permitted, including spaces (” “)
            // Payloads for type “rgb” contains 3 comma separated values of numbers with a valid range between 0 and 255. e.g. 100,100,100
            // Payloads for type “hsv” contains 3 comma separated values of numbers. The first number has a range of 0 to 360, the second and third numbers have a range of 0 to 100. e.g. 300,50,75
            // An empty string (“”) is not a valid payload
            auto value = Color{ColorFormat::HSV};
            assertTrue(value.validateValue("0,0,0"));
            assertTrue(value.validateValue("1,1,1"));
            assertTrue(value.validateValue("360,100,100"));
            assertFalse(value.validateValue(""));
            assertFalse(value.validateValue("0, 0, 0"));
            // assertFalse(value.validateValue("0,,0"));        // <- Is handled as 0,0,0. Not sure if I should really dismiss this value
                                                                // Next test case to ensure this behaviour
            auto a = Color{ColorFormat::HSV, "1,,3"};
            auto b = Color{ColorFormat::HSV, ColorTuple{1,0,3}};
            assertTrue(a == b);
            assertFalse(value.validateValue("0,0,0,"));
            assertFalse(value.validateValue("0.0.0"));
            assertFalse(value.validateValue("0.2,0.3,4.5"));
            assertFalse(value.validateValue("-1,0,0"));
            assertFalse(value.validateValue("0,-1,0"));
            assertFalse(value.validateValue("0,0,-1"));
            assertFalse(value.validateValue("361,0,0"));
            assertFalse(value.validateValue("0,101,0"));
            assertFalse(value.validateValue("0,0,101"));

            auto c = Color{ColorFormat::HSV, ColorTuple(4,5,6)};
            assertTrue(a != c);

            {
                auto d = Color{ColorFormat::HSV, ColorTuple(1,2,3)};
                assertTrue("1,2,3" == d.toString());
            }
            {
                auto d = Color{ColorFormat::HSV, "1,,4"};
                assertTrue("1,0,4" == d.toString());
            }

            {   
                auto successful = value.setValue("67,78,55");
                assertTrue(successful);
                assertTrue(value.value() == ColorTuple(67,78,55));
                assertTrue(value.toString() == "67,78,55");
                assertTrue(value.isValid());

                successful = value.setValue("300,78,112");
                assertFalse(successful);
                assertTrue(value.value() == ColorTuple(67,78,55));
                assertTrue(value.toString() == "67,78,55");
                assertTrue(value.isValid());

                successful = value.setValue("67,100,55");
                assertTrue(successful);
                assertTrue(value.value() == ColorTuple(67,100,55));
                assertTrue(value.toString() == "67,100,55");
                assertTrue(value.isValid());
            }
        }
    }
}