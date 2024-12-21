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
		SpriteComponent( TextureHandle a_Texture );
		~SpriteComponent() = default;

		TextureHandle GetTexture() { return m_Texture; }
		void SetTexture( AssetHandle a_Texture ) { m_Texture = a_Texture; }

	private:
		TextureHandle m_Texture;
	};

}