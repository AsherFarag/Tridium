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
		static Ref<Material> Load( const std::string& path );

		Material();
		Material( const Ref<Shader>& shader );

		void Bind();
		void Unbind();

		Ref<Shader> GetShader() const { return m_Shader; }
		void SetShader( const Ref<Shader>& shader ) { m_Shader = shader; }
		Ref<Material> GetParent() const { return m_Parent; }

		EBlendMode BlendMode;
		Color Color;
		float Reflectivity;
		float Refraction;

		Ref<Texture> BaseColorTexture;
		Ref<Texture> NormalMapTexture;
		Ref<Texture> MetallicTexture;
		Ref<Texture> RoughnessTexture;
		Ref<Texture> EmissiveTexture;

	private:
		Ref<Shader> m_Shader;
		Ref<Material> m_Parent;
	};
}