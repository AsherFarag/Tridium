#include "tripch.h"
#include "EnvironmentMapAsset.h"

namespace Tridium {
	
	bool EnvironmentMap::Valid() const
	{
		return m_Format != ERHIFormat::Unknown && !m_RadianceMips.Empty() && m_Irradiance.FaceWidth > 0;
	}

} // namespace Tridium