#pragma once
#include <Tridium/RHI/RHICommon.h>
#include <Tridium/RHI/DynamicRHI.h>
#include <Tridium/Core/Containers/Array.h>
#include <Tridium/Core/Containers/String.h>

namespace Tridium {

	class OpenGLRHI final : public DynamicRHI
	{
	public:
		virtual bool Init( const RHIConfig& a_Config ) override;
		virtual bool Shutdown() override;
		virtual bool Present() override;
		virtual ERHInterfaceType GetRHInterfaceType() const override final { return ERHInterfaceType::OpenGL; }

		virtual bool ExecuteCommandList( RHICommandListRef a_CommandList ) override;

		virtual RHIFence CreateFence() const override;
		virtual ERHIFenceState GetFenceState( RHIFence a_Fence ) const override;
		virtual void FenceSignal( RHIFence a_Fence ) override;

	#if RHI_DEBUG_ENABLED
		virtual void DumpDebug() override;
	#endif // RHI_DEBUG_ENABLED
	};

}