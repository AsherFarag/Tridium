#pragma once
#include "TextSerializer.h"

namespace Tridium {

	class Scene;

	namespace IO {

		template<>
		void SerializeToText( Archive& a_Archive, const Scene& a_Data );

		template<>
		bool DeserializeFromText( const YAML::Node& a_Node, Scene& a_Data );
	}
}