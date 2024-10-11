#pragma once
#include "Meta.h"

namespace Tridium::Refl {

	class MetaRegistry;

	template<typename T>
	class Reflector;

	enum class ETypeTraits : uint16_t
	{
		Is_Component = BIT(0),
	};
}

// To be declared in the class definition
#define REFLECT(Class)                                            \
	template<typename T> friend class ::Tridium::Refl::Reflector;