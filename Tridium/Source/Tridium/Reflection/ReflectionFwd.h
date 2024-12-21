// NOTE:
// Include this file in header files.
// Reflection.h is to be included in source files.
// This is to minimize compile times.

#pragma once

// Forward declarations
namespace entt {
	struct type_info;
	struct meta_data;
	struct meta_func;
	struct meta_prop;
	struct meta_any;
	struct meta_handle;

}
// -------------------

namespace Tridium::Refl {

	using MetaIDType = uint32_t;
	constexpr MetaIDType INVALID_META_ID = 0;

	// TypeInfo is a type that stores basic information about a type, such as it's name and identifier.
	// The identifier can be used to retrieve it's MetaType from the registry.
	using TypeInfo = entt::type_info;

	// A Property is a piece of data that is stored in a class.
	// Example: 
	// int MyInt
	// from
	// class MyClass { int MyInt; };
	using MetaProp = entt::meta_data;

	// Func can be any type of function, including member functions and functors.
	using MetaFunc = entt::meta_func;

	// MetaAttribute is a property that is stored in a class's metadata.
	// It can be any type of data, and be retrieved at runtime to perform various operations.
	// This can be used to store additional information about a class that is not directly related to the class itself.
	using MetaAttribute = entt::meta_prop;

	// MetaAny acts like an std::any, but also stores the MetaType of the data it holds.
	using MetaAny = entt::meta_any;

	// MetaHandle stores a non-owning pointer to an object with it's associated MetaType.
	// It cannot perform copies and does not prolong the lifetime of the object it points to.
	using MetaHandle = entt::meta_handle;

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