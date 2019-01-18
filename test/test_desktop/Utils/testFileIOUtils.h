#include <FileIOUtils.hpp>
#include <ArduinoUnit.h>

test(splitString) { 
    {
        auto values = splitString("123,456,789", ',');
        assertEqual(values.size(), 3);
        assertTrue(values[0] == "123");
        assertTrue(values[1] == "456");
        assertTrue(values[2] == "789");
    }
    {
        auto values = splitString("abc_def", '_');
        assertEqual(values.size(), 2);
        assertTrue(values[0] == "abc");
        assertTrue(values[1] == "def");
    }
}

test(toString) {
    assertTrue(to_string(1) == "1");
    assertTrue(to_string(1.0) == "1");
    assertTrue(to_string(1.2) == "1.2");
    assertTrue(to_string(1.) == "1");
    assertTrue(to_string(.0) == "0");
    assertTrue(to_string(.1) == "0.1");
    assertTrue(to_string(1e2) == "100");
    assertTrue(to_string(1E2) == "100");
}

test(toLower) {
    assertTrue(toLower("AAA") == "aaa");
    assertTrue(toLower("AbA") == "aba");
    assertTrue(toLower("AA:") == "aa:");
}

test(checkStringForAllowedCharacters) {
    std::string allowedCharacters = "0123456789,";
    assertTrue(checkStringForAllowedCharacters("12", allowedCharacters));
    assertFalse(checkStringForAllowedCharacters("12.3", allowedCharacters));
    assertFalse(checkStringForAllowedCharacters("12,3f", allowedCharacters));
}