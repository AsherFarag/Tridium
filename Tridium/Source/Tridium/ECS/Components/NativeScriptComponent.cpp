#include "tripch.h"
#include "NativeScriptComponent.h"
#include <Tridium/Reflection/Reflection.h>


namespace Tridium {
	BEGIN_REFLECT_COMPONENT( NativeScriptComponent )
		PROPERTY( m_Flags, Serialize )
	END_REFLECT_COMPONENT( NativeScriptComponent )
}