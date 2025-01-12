#pragma once
#include <Tridium/Core/Engine/EngineSubsystem.h>
#include <Tridium/Core/Singleton.h>
#include <Tridium/Core/Containers/BidirectionalMap.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Asset/AssetType.h>
#include "RHIResource.h"

namespace Tridium {

	class RenderResourceManager final : public IEngineSubsystem
	{
	public:
		
	private:
		BidirectionalMap<AssetHandle, SharedPtr<RHIResource>> m_Resources;
	};

}