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
		static SharedPtr<Material> Load( const std::string& path );

		Material();
		Material( const SharedPtr<Shader>& shader );

		void Bind();
		void Unbind();

		SharedPtr<Shader> GetShader() const { return m_Shader; }
		void SetShader( const SharedPtr<Shader>& shader ) { m_Shader = shader; }
		SharedPtr<Material> GetParent() const { return m_Parent; }

		EBlendMode BlendMode;
		Color Color;
		float Reflectivity;
		float Refraction;

		SharedPtr<Texture> BaseColorTexture;
		SharedPtr<Texture> NormalMapTexture;
		SharedPtr<Texture> MetallicTexture;
		SharedPtr<Texture> RoughnessTexture;
		SharedPtr<Texture> EmissiveTexture;

	private:
		SharedPtr<Shader> m_Shader;
		SharedPtr<Material> m_Parent;
	};
}