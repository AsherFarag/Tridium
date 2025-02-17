#include "tripch.h"
#include "D3D12CommandList.h"
#include "D3D12RHI.h"

namespace Tridium {

	bool D3D12CommandList::SetCommands( const RHICommandBuffer& a_CmdBuffer )
	{
		DirectX12RHI* rhi = RHI::GetDirectX12RHI();
		CHECK( rhi );

		const auto& cmdAllocator = rhi->GetCommandAllocator();
		// Reset the command allocator
		if ( FAILED( cmdAllocator->Reset() ) )
		{
			return false;
		}

		const auto& cmdList = rhi->GetCommandList();
		// Reset the command list
		if ( FAILED( cmdList->Reset( cmdAllocator.Get(), nullptr ) ) )
		{
			return false;
		}

		// Execute the commands
		for ( const RHICommand& cmd : a_CmdBuffer.Commands )
		{
			switch ( cmd.Type() )
			{
				using enum ERHICommandType;
			case SetPipelineState:
			{
				const auto& data = cmd.Get<SetPipelineState>();
				cmdList->SetPipelineState( data.PSO->NativePtrAs<ID3D12PipelineState*>() );
				break;
			}
			case SetRenderTargets:
			{
				const auto& data = cmd.Get<SetRenderTargets>();

			}
			case SetClearValues:
			{
				const auto& data = cmd.Get<SetClearValues>();
			}
			case ClearRenderTargets:
			{
				const auto& data = cmd.Get<ClearRenderTargets>();
			}
			}
		}

		return false;
	}

	bool D3D12CommandList::Commit( const void* a_Params )
	{
		return false;
	}

	bool D3D12CommandList::Release()
	{
		return false;
	}

	bool D3D12CommandList::IsValid() const
	{
		return false;
	}

	const void* D3D12CommandList::NativePtr() const
	{
		return nullptr;
	}

}