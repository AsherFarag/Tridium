#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class TagComponent : public Component
	{
		REFLECT( TagComponent );

	public:
		TagComponent();
		TagComponent( const std::string& a_Tag );
		TagComponent( std::string&& a_Tag );
		static inline const uint32_t MaxSize() { return 255u; }

		std::string Tag;
	};
}