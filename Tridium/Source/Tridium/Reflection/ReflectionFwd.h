#pragma once
// NOTE:
// Include this file in header files.
// Reflection.h is to be included in source files.
// This is to minimize compile times.

namespace Tridium::Refl {

	namespace Internal {
		template<typename T>
		struct Reflector {};

		// Used to reflect types inside files that are being ignored by the linker.
		// You can inlcude your unlinked types in "ReflectorInitializer.h"
		class HiddenTypeReflector;
	}
}

// To be declared in the class definition
// Example:
// class MyClass
// {
//     REFLECT(MyClass)
// public:
//    int MyInt;
// };

#define REFLECT(Class) \
	template<typename T> friend struct ::Tridium::Refl::Internal::Reflector; \
    static void __Tridium_Reflect(); \
    friend class ::Tridium::Refl::Internal::HiddenTypeReflector;