#pragma once
#include <Tridium/RHI/DynamicRHI.h>
#include <Tridium/Core/Containers/Array.h>
#include <Tridium/Core/Containers/String.h>

namespace Tridium::GL {

	class OpenGLRHI final : public DynamicRHI
	{
	public:
		virtual bool Init( const RHIConfig& a_Config ) override;
		virtual bool Shutdown() override;
		virtual bool Present() override;

	#if RHI_DEBUG_ENABLED
		virtual void DumpDebug() override;
	#endif // RHI_DEBUG_ENABLED
	};

}