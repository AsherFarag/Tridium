#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Shader.h>

namespace Tridium {
	class Texture2D;

	DEFINE_COMPONENT( SpriteComponent )
	{
	public:
		SpriteComponent();
		SpriteComponent(const Ref<Texture2D>& a_Texture);
		SpriteComponent( const std::string& a_TexturePath );
		~SpriteComponent() = default;

		inline Ref<Texture2D>& GetTexture() { return m_Texture; }
		inline void SetTexture(const  Ref<Texture2D>& a_Texture ) { m_Texture = a_Texture; }
		inline Ref<Shader>& GetShader() { return m_Shader; }

	private:
		Ref<Shader> m_Shader;
		Ref<Texture2D> m_Texture;
	};

}