#include "tripch.h"
#include "SpriteComponent.h"

namespace Tridium {

	BEGIN_REFLECT_COMPONENT( SpriteComponent )
		BASE( Component )
		PROPERTY( m_Texture, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( SpriteComponent )

	SpriteComponent::SpriteComponent( const AssetRef<Texture>& a_Texture )
		: m_Texture( a_Texture )
	{
	}
}