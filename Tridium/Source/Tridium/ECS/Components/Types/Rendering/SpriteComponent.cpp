#include "tripch.h"
#include "SpriteComponent.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {
	BEGIN_REFLECT_COMPONENT( SpriteComponent, Scriptable )
		BASE( Component )
		PROPERTY( m_Texture, Serialize | EditAnywhere )
	END_REFLECT_COMPONENT( SpriteComponent )

	SpriteComponent::SpriteComponent( TextureHandle a_Texture )
		: m_Texture( a_Texture )
	{
	}
}