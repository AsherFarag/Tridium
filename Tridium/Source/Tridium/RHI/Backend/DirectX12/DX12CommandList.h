#pragma once
#include "DirectX12.h"
#include <Tridium/RHI/RHICommandList.h>

namespace Tridium {

	class DX12CommandList final : public RHICommandList
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );

		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		bool SetCommands( const RHICommandBuffer& a_CmdBuffer ) override;
	};

}