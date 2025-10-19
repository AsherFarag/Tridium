#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class OldTagComponent : public Component
	{
		REFLECT( OldTagComponent );

	public:
		OldTagComponent();
		OldTagComponent( const std::string& a_Tag );
		OldTagComponent( std::string&& a_Tag );
		static inline const uint32_t MaxSize() { return 255u; }

		std::string Tag;
	};
}