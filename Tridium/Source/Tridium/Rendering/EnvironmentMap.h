#pragma once
#include <Tridium/IO/FilePath.h>
#include <Tridium/Rendering/Texture.h>

namespace Tridium {

	class EnvironmentMap
	{
	public:
		static SharedPtr<EnvironmentMap> Create( const SharedPtr<Texture>& a_EquirectangularTexture );
		static SharedPtr<EnvironmentMap> Create( const IO::FilePath& a_Path );
		static SharedPtr<EnvironmentMap> Create( AssetHandle a_Handle );

		SharedPtr<CubeMap> GetIrradianceMap() const { return m_IrradianceMap; }
		SharedPtr<CubeMap> GetRadianceMap() const { return m_RadianceMap; }

	protected:
		SharedPtr<CubeMap> m_IrradianceMap;
		SharedPtr<CubeMap> m_RadianceMap;
	};
}