#pragma once
#include "Component.h"
#include <Tridium/Core/EnumFlags.h>

namespace Tridium {

	//================================================================
	// GameObject Flags
	//  Bit flags used to represent the state of a GameObject.
	namespace EGameObjectFlag {
		enum Type : uint32_t
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
	}
	using GameObjectFlags = EnumFlags<EGameObjectFlag::Type>;
	static constexpr GameObjectFlags DefaultGameObjectFlags{ EGameObjectFlag::Active | EGameObjectFlag::Visible };
	//================================================================

	class GameObjectFlagsComponent : public Component
	{
	public:
		COMPONENT_BODY( GameObjectFlagsComponent, Component );
		GameObjectFlags Flags{ EComponentFlags::Active | EComponentFlags::Visible };

		GameObjectFlagsComponent() = default;
		GameObjectFlagsComponent( const GameObjectFlags& a_Flags )
			: Flags( a_Flags ) {}
	};

} // namespace Tridium