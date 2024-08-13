#include "tripch.h"
#include "SpriteComponent.h"

namespace Tridium {
	SpriteComponent::SpriteComponent( const AssetRef<Texture>& a_Texture )
		: m_Texture( a_Texture )
	{
	}
}