#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Asset/TextureAsset.h>
#include <Tridium/Graphics/Color.h>

namespace Tridium {

	namespace DefaultShaderFamilies 
	{
		inline constexpr StringView Unlit = "Unlit";
		inline constexpr StringView Lit = "Lit";
		inline constexpr StringView Toon = "Toon";
	}

	//=================================================================================================
	// Material Flags: Defines various properties of a material that effect how it is rendered.
	//=================================================================================================
	enum class EMaterialFlags : uint32_t
	{
		None = 0,
		// The material is rendered on both sides of the polygons. Aka, it does not cull back faces.
		TwoSided = 1 << 0,
		// The material is transparent and should be rendered with alpha blending.
		Transparent = 1 << 1,
		// The material does not cast shadows.
		DisableShadowCasting = 1 << 2,
	};
	DEFINE_ENUM_BITMASK_OPERATORS( EMaterialFlags );

	//=================================================================================================
	// Material: A pipeline state and set of textures that define how a mesh is rendered.
	//=================================================================================================
	DEFINE_ASSET_TYPE( Material )
	{
	public:

		//=============================================================================================
		bool Valid() const override { return true; }

		auto Flags() const { return m_Flags; }
		void SetFlags( const EMaterialFlags a_Flags ) { m_Flags = a_Flags; }
		const auto& ShaderFamily() const { return m_ShaderFamily; }
		void SetShaderFamily( String a_Family ) { m_ShaderFamily = std::move( a_Family ); }

		const auto& AlbedoMap() const { return m_AlbedoMap; }
		const auto& NormalMap() const { return m_NormalMap; }
		const auto& MetallicMap() const { return m_MetallicMap; }
		const auto& RoughnessMap() const { return m_RoughnessMap; }
		const auto& EmissiveMap() const { return m_EmissiveMap; }
		const auto& AmbientOcclusionMap() const { return m_AmbientOcclusionMap; }
		const auto& OpacityMap() const { return m_OpacityMap; }
		Color AlbedoColor() const { return m_AlbedoColor; }
		float MetallicIntensity() const { return m_MetallicIntensity; }
		float RoughnessIntensity() const { return m_RoughnessIntensity; }
		float EmissiveIntensity() const { return m_EmissiveIntensity; }

		void SetAlbedoMap( AssetRef<Texture> a_Texture ) { m_AlbedoMap = std::move( a_Texture ); }
		void SetNormalMap( AssetRef<Texture> a_Texture ) { m_NormalMap = std::move( a_Texture ); }
		void SetMetallicMap( AssetRef<Texture> a_Texture ) { m_MetallicMap = std::move( a_Texture ); }
		void SetRoughnessMap( AssetRef<Texture> a_Texture ) { m_RoughnessMap = std::move( a_Texture ); }
		void SetEmissiveMap( AssetRef<Texture> a_Texture ) { m_EmissiveMap = std::move( a_Texture ); }
		void SetAmbientOcclusionMap( AssetRef<Texture> a_Texture ) { m_AmbientOcclusionMap = std::move( a_Texture ); }
		void SetOpacityMap( AssetRef<Texture> a_Texture ) { m_OpacityMap = std::move( a_Texture ); }
		void SetAlbedoColor( Color a_Color ) { m_AlbedoColor = a_Color; }
		void SetMetallicIntensity( float a_Intensity ) { m_MetallicIntensity = a_Intensity; }
		void SetRoughnessIntensity( float a_Intensity ) { m_RoughnessIntensity = a_Intensity; }
		void SetEmissiveIntensity( float a_Intensity ) { m_EmissiveIntensity = a_Intensity; }

		bool Transparent() const { return m_Flags.HasFlag( EMaterialFlags::Transparent ); }
		bool CastsShadows() const { return !m_Flags.HasFlag( EMaterialFlags::DisableShadowCasting ); }

	protected:

		//=============================================================================================
		String m_ShaderFamily;
		EnumFlags<EMaterialFlags> m_Flags{ EMaterialFlags::None };

		//=============================================================================================
		// Material Properties
		AssetRef<Texture> m_AlbedoMap;
		AssetRef<Texture> m_NormalMap;
		AssetRef<Texture> m_MetallicMap;
		AssetRef<Texture> m_RoughnessMap;
		AssetRef<Texture> m_EmissiveMap;
		AssetRef<Texture> m_AmbientOcclusionMap;
		AssetRef<Texture> m_OpacityMap;

		Color m_AlbedoColor = Color::White();
		float m_MetallicIntensity = 1.0f;
		float m_RoughnessIntensity = 1.0f;
		float m_EmissiveIntensity = 1.0f;

	};

} // namespace Tridium