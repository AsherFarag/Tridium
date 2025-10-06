#pragma once
#include <Tridium/IO/FilePath.h>
#include <Tridium/Graphics/oldRendering/Texture.h>

namespace Tridium {

	class EnvironmentMapOld
	{
	public:
		static SharedPtr<EnvironmentMapOld> Create( const SharedPtr<TextureOld>& a_EquirectangularTexture );
		static SharedPtr<EnvironmentMapOld> Create( const FilePath& a_Path );
		static SharedPtr<EnvironmentMapOld> Create( AssetHandle a_Handle );

		SharedPtr<CubeMap> GetIrradianceMap() const { return m_IrradianceMap; }
		SharedPtr<CubeMap> GetRadianceMap() const { return m_RadianceMap; }

	protected:
		SharedPtr<CubeMap> m_IrradianceMap;
		SharedPtr<CubeMap> m_RadianceMap;
	};
}