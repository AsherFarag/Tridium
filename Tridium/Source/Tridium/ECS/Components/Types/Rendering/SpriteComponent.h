#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Texture.h>

namespace Tridium {

	DEFINE_COMPONENT( SpriteComponent )
	{
	public:
		SpriteComponent() = default;
		SpriteComponent( const SharedPtr<Texture>&texture );
		~SpriteComponent() = default;

		const SharedPtr<Texture>& GetTexture() { return m_Texture; }
		void SetTexture(const SharedPtr<Texture>& texture ) { m_Texture = texture; }

	private:
		SharedPtr<Texture> m_Texture;
	};

}