#ifndef __ARDUINOIOSTREAM_H__
#define __ARDUINOIOSTREAM_H__

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 

static const String endl = "\n";

#endif /* __ARDUINOIOSTREAM_H__ */