#pragma once
#include "D3D12Common.h"
#include <Tridium/Graphics/RHI/RHICommandList.h>

namespace Tridium {

	class D3D12CommandList final : public RHICommandList
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );

		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		bool SetCommands( const RHICommandBuffer& a_CmdBuffer ) override;

		TODO( "Seperate cmd lists into graphics and compute" );
		ComPtr<D3D12::GraphicsCommandList> CommandList;
	};

}