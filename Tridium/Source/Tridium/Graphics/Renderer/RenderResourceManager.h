#pragma once
#include <Tridium/Containers/Map.h>
#include <Tridium/Asset/AssetDefinitions.h>
#include <Tridium/Graphics/RHI/RHIResource.h>

namespace Tridium {

	struct RenderResource
	{
		
	};

	class RenderResourceManager
	{

	private:
		UnorderedMap<T::AssetID, RHIResourceWeakRef> m_AssetToRHIMap;
	};

}  // namespace Tridium