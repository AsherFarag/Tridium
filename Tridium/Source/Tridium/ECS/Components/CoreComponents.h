#pragma once
#include "Component.h"

namespace Tridium {

	class GameObjectFlagsComponent : public Component
	{
	public:
		COMPONENT_BODY( GameObjectFlagsComponent, Component );
		GameObjectFlags Flags;

		GameObjectFlagsComponent() = default;
		GameObjectFlagsComponent( const GameObjectFlags& a_Flags )
			: Flags( a_Flags ) {}
	};

} // namespace Tridium