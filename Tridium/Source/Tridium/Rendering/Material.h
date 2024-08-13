#pragma once
#include <Tridium/Core/Asset.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Shader.h>

namespace Tridium {

	enum class EBlendMode
	{
		Additive = 0, Subtractive, Multiplicative, Alpha
	};

	class Material : public Asset
	{
		friend class MaterialSerializer;

	public:
		ASSET_CLASS_TYPE( Material )

		Material();
		Material( const AssetRef<Shader>& a_Shader );

		void Bind();
		void Unbind();

		AssetRef<Shader> GetShader() const { return m_Shader; }
		void SetShader( const AssetRef<Shader>& a_Shader ) { m_Shader = a_Shader; }
		AssetRef<Material> GetParent() const { return m_Parent; }

		EBlendMode BlendMode;
		Color Color;
		float Reflectivity;
		float Refraction;

		AssetRef<Texture> BaseColorTexture;
		AssetRef<Texture> NormalMapTexture;
		AssetRef<Texture> MetallicTexture;
		AssetRef<Texture> RoughnessTexture;
		AssetRef<Texture> EmissiveTexture;

	private:
		AssetRef<Shader> m_Shader;
		AssetRef<Material> m_Parent;
	};
}