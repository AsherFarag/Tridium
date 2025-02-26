#pragma once
#include "Component.h"
#include <Tridium/Core/Enum.h>

namespace Tridium {

	//================================================================
	// GameObject Flags
	//  Bit flags used to represent the state of a GameObject.
	enum class EGameObjectFlags : uint32_t
	{
		None = 0,
		// Is this GameObject pending destruction?
		PendingKill = 1 << 0,
		// If not active, this GameObject will not be updated.
		Active = 1 << 1,
		// If not visible, this GameObject will not be rendered.
		Visible = 1 << 2,

		// The GameObject is enabled if it is both active and visible.
		Enabled = Active | Visible,
	};
	ENUM_ENABLE_BITMASK_OPERATORS( EGameObjectFlags );
	//================================================================

	class GameObjectFlagsComponent : public Component
	{
	public:
		COMPONENT_BODY( GameObjectFlagsComponent, Component );
		EnumFlags<EGameObjectFlags> Flags{ EComponentFlags::Active | EComponentFlags::Visible };

		GameObjectFlagsComponent() = default;
		GameObjectFlagsComponent( EGameObjectFlags a_Flags )
			: Flags( a_Flags ) {}
	};

} // namespace Tridium