#pragma once

namespace Tridium::Reflection {

	class MetaRegistry;

	template<typename T>
	class Reflector;
}

// To be declared in the class definition
#define REFLECT friend class SceneSerializer;\
				template<typename T>\
                friend class ::Tridium::Reflection::Reflector;\
                static void OnReflect();