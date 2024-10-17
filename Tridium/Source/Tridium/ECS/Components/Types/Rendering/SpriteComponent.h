#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Texture.h>

namespace Tridium {

	class Texture;

	class SpriteComponent : public Component
	{
		REFLECT( SpriteComponent );
	public:
		SpriteComponent() = default;
		SpriteComponent( AssetHandle a_Texture )
			: m_Texture( a_Texture ) {}
		~SpriteComponent() = default;

		AssetHandle GetTexture() { return m_Texture; }
		void SetTexture( AssetHandle a_Texture ) { m_Texture = a_Texture; }

	private:
		AssetHandle m_Texture;
	};

	BEGIN_REFLECT_COMPONENT( SpriteComponent )
		BASE( Component )
		PROPERTY( m_Texture, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( SpriteComponent )

}