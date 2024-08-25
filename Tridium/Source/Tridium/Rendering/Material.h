#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Shader.h>
#include <variant>

namespace Tridium {

	enum class EBlendMode : uint8_t
	{
		Additive = 0, Subtractive, Multiplicative, Alpha
	};

	enum class EPropertyType : uint8_t
	{
		Int, IntArray,
		Float, FloatArray,
		Color, ColorArray,
		Vector4, Vector4Array,
		Matrix4, Matrix4Array,
		Texture,
		None
	};

	struct Property
	{
		EPropertyType Type = EPropertyType::None;
		std::variant<
			int, std::vector<int>,
			float, std::vector<float>, 
			Color, std::vector<Color>,
			Vector4, std::vector<Vector4>,
			Matrix4, std::vector<Matrix4>,
			AssetRef<Texture>> Value;
	};

	class Material : public Asset
	{
		friend class MaterialSerializer;
	public:
		ASSET_CLASS_TYPE( Material )
		ASSET_LOADER_TYPE( MaterialLoader )

		Material();
		Material( const AssetRef<Shader>& a_Shader );

		void Bind();
		void Unbind();

		const AssetRef<Shader>& GetShader() const { return m_Shader; }
		void SetShader( const AssetRef<Shader>& a_Shader ) { m_Shader = a_Shader; }
		const AssetRef<Material>& GetParent() const { return m_Parent; }

		bool AddProperty( const std::string& a_Name, const Property& a_Property );
		bool RemoveProperty( const std::string& a_Name );

		// - Getters -

		int*			      GetInt( const std::string& a_Name );
		std::vector<int>*     GetIntVector( const std::string& a_Name );
		float*                GetFloat( const std::string& a_Name );
		std::vector<float>*   GetFloatArray( const std::string& a_Name );
		Color*                GetColor( const std::string& a_Name );
		std::vector<Color>*   GetColorArray( const std::string& a_Name );
		Vector4*              GetVector4( const std::string& a_Name );
		std::vector<Vector4>* GetVector4Array( const std::string& a_Name );
		Matrix4*              GetMatrix4( const std::string& a_Name );
		std::vector<Matrix4>* GetMatrix4Array( const std::string& a_Name );
		AssetRef<Texture>*    GetTexture( const std::string& a_Name );

		// - Setters -

		bool SetInt( const std::string& a_Name, int a_Value );
		bool SetIntArray( const std::string& a_Name, const std::vector<int>& a_Value );
		bool SetFloat( const std::string& a_Name, float a_Value );
		bool SetFloatArray( const std::string& a_Name, const std::vector<float>& a_Value );
		bool SetColor( const std::string& a_Name, const Color& a_Value );
		bool SetColorArray( const std::string& a_Name, const std::vector<Color>& a_Value );
		bool SetVector4( const std::string& a_Name, const Vector4& a_Value );
		bool SetVector4Array( const std::string& a_Name, const std::vector<Vector4>& a_Value );
		bool SetMatrix4( const std::string& a_Name, const Matrix4& a_Value );
		bool SetMatrix4Array( const std::string& a_Name, const std::vector<Matrix4>& a_Value );
		bool SetTexture( const std::string& a_Name, const AssetRef<Texture>& a_Value );

		EBlendMode BlendMode;

	private:
		AssetRef<Shader> m_Shader;
		AssetRef<Material> m_Parent;
		std::unordered_map<std::string, Property> m_Properties;
	};
}