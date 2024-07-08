#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Texture.h>

namespace Tridium {

	DEFINE_COMPONENT( SpriteComponent )
	{
	public:
		SpriteComponent();
		SpriteComponent( const TextureHandle& a_Texture );
		~SpriteComponent() = default;

		inline TextureHandle GetTexture() { return m_Texture; }
		inline void SetTexture(const  TextureHandle& a_Texture ) { m_Texture = a_Texture; }
		inline Ref<Shader>& GetShader() { return m_Shader; }

	private:
		Ref<Shader> m_Shader;
		TextureHandle m_Texture;
	};

}