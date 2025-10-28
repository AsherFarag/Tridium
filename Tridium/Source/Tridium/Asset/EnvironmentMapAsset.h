#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Core/Core.h>
#include <Tridium/Graphics/RHI/RHIDefinitions.h>

namespace Tridium {

	//=================================================================================================
	// Environment Map Asset:
	// A preprocessed environment map containing irradiance and radiance cubemaps.
	//=================================================================================================
	DEFINE_ASSET_TYPE( EnvironmentMap )
	{
	public:

		//=============================================================================================
		// CubeMap: A single mip level of a cubemap texture.
		//=============================================================================================
		struct CubeMap
		{
			Array<byte_t> PixelData;
			uint32_t FaceWidth = 0u;
			uint32_t RowStride = 0u;
			uint32_t FaceStride = 0u;
		};

		//=============================================================================================
		bool Valid() const override { return m_Format != ERHIFormat::Unknown && !m_RadianceMips.Empty() && m_Irradiance.FaceWidth > 0; }

		//=============================================================================================
		// Returns the RHI format for all pixels in the cubemaps.
		ERHIFormat Format() const { return m_Format; }

		//=============================================================================================
		// Size of the base mip level of the radiance cubemap (width and height of each face).
		uint32_t RadianceSize() const { return m_RadianceMips.Empty() ? 0 : m_RadianceMips[0].FaceWidth; }

		//=============================================================================================
		// Size of the diffuse irradiance cubemap (width and height of each face).
		uint32_t IrradianceSize() const { return m_Irradiance.FaceWidth; }

		//=============================================================================================
		// Returns the number of radiance mip levels.
		size_t NumRadianceMips() const { return m_RadianceMips.Size(); }

		//=============================================================================================
		// Returns the array of radiance mip levels, from largest to smallest.
		const Array<CubeMap>& RadianceMipMaps() const { return m_RadianceMips; }

		//=============================================================================================
		// Returns the diffuse irradiance cubemap.
		const CubeMap& IrradianceMap() const { return m_Irradiance; }

	protected:

		//=============================================================================================
		// RHI format of the cubemaps
		ERHIFormat m_Format = ERHIFormat::Unknown;
		// The radiance/specular cubemap mips.
		Array<CubeMap> m_RadianceMips;
		// The irradiance/diffuse cubemap
		CubeMap m_Irradiance;

	};

} // namespace Tridium