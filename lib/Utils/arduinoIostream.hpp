#ifndef __ARDUINOIOSTREAM_H__
#define __ARDUINOIOSTREAM_H__

#include <Arduino.h>
#include <string>

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 
inline Print &operator <<(Print &obj, std::string arg) { obj.print(arg.c_str()); return obj; } 

static const std::string endl = "\n";

#endif /* __ARDUINOIOSTREAM_H__ */