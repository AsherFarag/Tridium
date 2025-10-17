#pragma once
#include "TextSerializer.h"

namespace Tridium {

	class OldScene;

	namespace IO {

		template<>
		void SerializeToText( Archive& a_Archive, const OldScene& a_Data );

		template<>
		bool DeserializeFromText( const YAML::Node& a_Node, OldScene& a_Data );
	}
}