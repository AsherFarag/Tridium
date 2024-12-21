// NOTE:
// This file should only be included in source files.
// This is not required but is recommended to minimize compile times.
// Include ReflectionFwd.h in header files instead to declare a class as reflectable.
// 
// Example .h:
// class MyClass
// {
//     REFLECT(MyClass)
// public:
//    int MyInt;
//    void MyFunc() {}
// };
// 
// Example .cpp:
// #include "MyClass.h"
// #include "Reflection.h"
// BEGIN_REFLECT(MyClass)
//	  PROPERTY(MyInt)
//    FUNCTION(MyFunc)
//	  META("MyExtraMetaData", "MyValue")
// END_REFLECT(MyClass)

#pragma once
#include "ReflectionImpl.h"

#define BEGIN_REFLECT(...) _BEGIN_REFLECT_SELECTOR(__VA_ARGS__)
	#define BASE(...) _BASE(__VA_ARGS__)
	#define PROPERTY(...) _PROPERTY_SELECTOR(__VA_ARGS__)
	#define FUNCTION(...) _FUNCTION_SELECTOR(__VA_ARGS__)
	#define META( Key, Value ) _META( Key, Value )
	#define OVERRIDE( MetaProp, Override ) _OVERRIDE( MetaProp, Override )
#define END_REFLECT( Class ) _END_REFLECT( Class )

#define BEGIN_REFLECT_COMPONENT(...) _BEGIN_REFLECT_COMPONENT_SELECTOR(__VA_ARGS__)
#define END_REFLECT_COMPONENT( Class ) _END_REFLECT_COMPONENT( Class )

#define BEGIN_REFLECT_ENUM( Enum ) _BEGIN_REFLECT_ENUM(Enum)
	#define ENUM_VALUE( Value ) _ENUM_VALUE(Value)
#define END_REFLECT_ENUM( Enum ) _END_REFLECT_ENUM(Enum)