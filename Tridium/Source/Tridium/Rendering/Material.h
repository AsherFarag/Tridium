#pragma once
#include <Tridium/Core/Asset.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Shader.h>

namespace Tridium {

	using MaterialHandle = AssetHandle;

	enum class EBlendMode
	{
		Additive = 0, Subtractive, Multiplicative, Alpha
	};

	class Material : public Asset
	{
		friend class MaterialSerializer;

	public:
		Material();
		Material( const ShaderHandle& shader );
		~Material() = default;

		void Bind();
		void Unbind();

		ShaderHandle GetShader() const { return m_Shader; }
		void SetShader( const ShaderHandle& shader ) { m_Shader = shader; }
		MaterialHandle GetParent() const { return m_Parent; }

		void SetTexture( const std::string& name, const TextureHandle& texture ) { m_Textures[name] = texture; }
		TextureHandle GetTexture( const std::string& name ) const;
		bool GetTexture( const std::string& name, TextureHandle& outTextureHandle ) const;

		EBlendMode BlendMode;
		Color Color;
		float Reflectivity;
		float Refraction;

		TextureHandle BaseColorTexture;
		TextureHandle NormalMapTexture;
		TextureHandle MetallicTexture;
		TextureHandle RoughnessTexture;
		TextureHandle EmissiveTexture;

	private:
		ShaderHandle m_Shader;
		MaterialHandle m_Parent;
		std::unordered_map<std::string, TextureHandle> m_Textures;
	};

	class MaterialLoader
	{
	public:
		static Ref<Material> Import( const std::string& filePath );
	};

	class MaterialLibrary : public AssetLibrary<MaterialLibrary, MaterialHandle, Material>
	{
	public:
		static inline Ref<Material> GetMaterial( const MaterialHandle& materialHandle ) { return Get().GetAsset( materialHandle ); }
		static inline bool GetMaterialHandle( const std::string& path, MaterialHandle& outMaterialHandle ) { return Get().GetGUID( path, outMaterialHandle ); }
		static inline MaterialHandle GetMaterialHandle( const std::string& path ) { return Get().GetGUID( path ); }
		static inline bool HasMaterialHandle( const std::string& path ) { return Get().HasHandle( path ); }
		static inline bool AddMaterial( const std::string& path, const Ref<Material>& material ) { return Get().AddAsset( path, material ); }
		static inline bool RemoveMaterial( const MaterialHandle& materialHandle ) { return Get().RemoveAsset( materialHandle ); }
	};

}