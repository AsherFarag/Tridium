#pragma once
#include "Asset.h"
#include <Tridium/Graphics/Color.h>

namespace Tridium::T {

	// Forward declarations
	class Texture;

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
	class Material : public IAsset
	{
	public:

		static SharedPtr<Material> Create() { return MakeShared<EnableMakeShared<Material>>(); }
		static constexpr EAssetType StaticType() { return EAssetType::Material; }
		EAssetType Type() const override { return StaticType(); }
		bool Valid() const override { return true; }

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

		void SetAlbedoMap( SharedPtr<Texture> a_Texture ) { m_AlbedoMap = std::move( a_Texture ); }
		void SetNormalMap( SharedPtr<Texture> a_Texture ) { m_NormalMap = std::move( a_Texture ); }
		void SetMetallicMap( SharedPtr<Texture> a_Texture ) { m_MetallicMap = std::move( a_Texture ); }
		void SetRoughnessMap( SharedPtr<Texture> a_Texture ) { m_RoughnessMap = std::move( a_Texture ); }
		void SetEmissiveMap( SharedPtr<Texture> a_Texture ) { m_EmissiveMap = std::move( a_Texture ); }
		void SetAmbientOcclusionMap( SharedPtr<Texture> a_Texture ) { m_AmbientOcclusionMap = std::move( a_Texture ); }
		void SetOpacityMap( SharedPtr<Texture> a_Texture ) { m_OpacityMap = std::move( a_Texture ); }
		void SetAlbedoColor( Color a_Color ) { m_AlbedoColor = a_Color; }
		void SetMetallicIntensity( float a_Intensity ) { m_MetallicIntensity = a_Intensity; }
		void SetRoughnessIntensity( float a_Intensity ) { m_RoughnessIntensity = a_Intensity; }
		void SetEmissiveIntensity( float a_Intensity ) { m_EmissiveIntensity = a_Intensity; }

		auto Flags() const { return m_Flags; }
		void SetFlags( const EMaterialFlags a_Flags ) { m_Flags = a_Flags; }

		bool Transparent() const { return m_Flags.HasFlag( EMaterialFlags::Transparent ); }
		bool CastsShadows() const { return !m_Flags.HasFlag( EMaterialFlags::DisableShadowCasting ); }

	protected:

		SharedPtr<Texture> m_AlbedoMap;
		SharedPtr<Texture> m_NormalMap;
		SharedPtr<Texture> m_MetallicMap;
		SharedPtr<Texture> m_RoughnessMap;
		SharedPtr<Texture> m_EmissiveMap;
		SharedPtr<Texture> m_AmbientOcclusionMap;
		SharedPtr<Texture> m_OpacityMap;

		Color m_AlbedoColor = Color::White();
		float m_MetallicIntensity = 1.0f;
		float m_RoughnessIntensity = 1.0f;
		float m_EmissiveIntensity = 1.0f;
		EnumFlags<EMaterialFlags> m_Flags{ EMaterialFlags::None };

	};

} // namespace Tridium