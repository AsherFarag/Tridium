#pragma once

namespace Tridium::Refl {

	class MetaRegistry;

	template<typename T>
	class Reflector;
}

// To be declared in the class definition
#define REFLECT template<typename T> friend class ::Tridium::Refl::Reflector;