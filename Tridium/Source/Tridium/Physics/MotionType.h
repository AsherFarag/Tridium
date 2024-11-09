#pragma once
#include <Tridium/Core/Types.h>
#include <Tridium/Utils/Reflection/Reflection.h>

namespace Tridium {

	enum class EMotionType : uint8_t
	{
		Static,
		Kinematic,
		Dynamic,
	};

	BEGIN_REFLECT_ENUM( EMotionType )
		ENUM_VAL( Static )
		ENUM_VAL( Kinematic )
		ENUM_VAL( Dynamic )
	END_REFLECT_ENUM( EMotionType )
}