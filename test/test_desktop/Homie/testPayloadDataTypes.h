#include <PayloadDataTypes.h>
#include <AUnit.h>

test(Homie_PayploadDataTypes, Integer) {
    // Integer types are UTF-8 encoded string literal representations of 64-bit signed whole numbers
    // Integers range from -9,223,372,036,854,775,808 (-263) to 9,223,372,036,854,775,807 (263-1)
    // The payload may only contain whole numbers and the negation character “-”. No other characters including spaces (” “) are permitted
    // A string with just a negation sign (“-”) is not a valid payload
    // An empty string (“”) is not a valid payload
    Rovi::Homie::Integer intValue(0);

    assertTrue(intValue.validateValue("123"));
    assertTrue(intValue.validateValue("-123"));
    assertFalse(intValue.validateValue("123.456"));
    assertFalse(intValue.validateValue("abc"));
    assertFalse(intValue.validateValue("-"));
    assertFalse(intValue.validateValue(""));
}