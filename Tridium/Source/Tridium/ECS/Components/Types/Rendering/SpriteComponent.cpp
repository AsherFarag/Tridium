#include "tripch.h"
#include "SpriteComponent.h"

namespace Tridium {
	SpriteComponent::SpriteComponent( const SharedPtr<Texture>& texture )
		: m_Texture( texture )
	{
	}
}