#pragma once
#include "Asset.h"
#include <Tridium/Graphics/Color.h>

namespace Tridium::T {

	// Forward declarations
	class Texture;

	class MaterialAsset : public IAsset
	{
	public:
		MaterialAsset( Private ) : IAsset( Private{} ) {}
		static SharedPtr<MaterialAsset> Create() { return MakeShared<MaterialAsset>( Private{} ); }
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
	};

}