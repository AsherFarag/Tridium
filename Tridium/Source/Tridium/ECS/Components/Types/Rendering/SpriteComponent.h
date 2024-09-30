#pragma once
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/Rendering/Texture.h>

namespace Tridium {

	class Texture;

	DEFINE_COMPONENT( SpriteComponent )
	{
		REFLECT;
	public:
		SpriteComponent() = default;
		SpriteComponent( const AssetRef<Texture>& a_Texture );
		~SpriteComponent() = default;

		const AssetRef<Texture>& GetTexture() { return m_Texture; }
		void SetTexture(const AssetRef<Texture>& a_Texture ) { m_Texture = a_Texture; }

	private:
		AssetRef<Texture> m_Texture;
	};

}