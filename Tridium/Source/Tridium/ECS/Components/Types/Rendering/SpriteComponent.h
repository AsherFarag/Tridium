#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Texture.h>

namespace Tridium {

	DEFINE_COMPONENT( SpriteComponent )
	{
	public:
		SpriteComponent() = default;
		SpriteComponent( const Ref<Texture>&texture );
		~SpriteComponent() = default;

		const Ref<Texture>& GetTexture() { return m_Texture; }
		void SetTexture(const Ref<Texture>& texture ) { m_Texture = texture; }

	private:
		Ref<Texture> m_Texture;
	};

}