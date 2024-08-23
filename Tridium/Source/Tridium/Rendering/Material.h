#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Shader.h>
#include <variant>

namespace Tridium {

	enum class EBlendMode
	{
		Additive = 0, Subtractive, Multiplicative, Alpha
	};

	enum class EPropertyType
	{
		Int, UInt32, Float, Double, Color, Vector2, Vector3, Vector4, Texture, None
	};

	struct Property
	{
		EPropertyType Type = EPropertyType::None;
		std::variant<int, uint32_t, float, double, Color, Vector2, Vector3, Vector4, AssetRef<Texture>> Value;
	};

	class Material : public Asset
	{
		friend class MaterialSerializer;
	public:
		ASSET_CLASS_TYPE( Material )
		ASSET_LOADER_TYPE( MaterialLoader );

		Material();
		Material( const AssetRef<Shader>& a_Shader );

		void Bind();
		void Unbind();

		AssetRef<Shader> GetShader() const { return m_Shader; }
		void SetShader( const AssetRef<Shader>& a_Shader ) { m_Shader = a_Shader; }
		AssetRef<Material> GetParent() const { return m_Parent; }

		bool AddProperty( const std::string& a_Name, const Property& a_Property );
		bool SetProperty( const std::string& a_Name, const Property& a_Property );
		Property* GetProperty( const std::string& a_Name );

		EBlendMode BlendMode;
		Color Color;

	private:
		AssetRef<Shader> m_Shader;
		AssetRef<Material> m_Parent;
		std::unordered_map<std::string, Property> m_Properties;
	};
}