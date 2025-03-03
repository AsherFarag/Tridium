#include "tripch.h"
#include "D3D12CommandList.h"
#include "D3D12PipelineState.h"
#include "D3D12Sampler.h"
#include "D3D12Texture.h"
#include "D3D12Mesh.h"
#include "D3D12DynamicRHI.h"
#include "D3D12ShaderBindingLayout.h"

namespace Tridium {

	bool D3D12CommandList::SetCommands( const RHICommandBuffer& a_CmdBuffer )
	{
		D3D12RHI* rhi = RHI::GetD3D12RHI();
		CHECK( rhi );

		const auto& cmdAllocator = rhi->GetCommandAllocator();
		// Reset the command allocator
		if ( FAILED( cmdAllocator->Reset() ) )
		{
			return false;
		}

		// Reset the command list
		if ( FAILED( CommandList->Reset( cmdAllocator.Get(), nullptr ) ) )
		{
			return false;
		}

		//TEMP?
		DescriptorHeaps.Clear();
		ShaderInputOffset = 0;

		// Execute the commands
		for ( const RHICommand& cmd : a_CmdBuffer.Commands )
		{
			switch ( cmd.Type() )
			{
			#define PerformCmd( _CmdType ) case ERHICommandType::_CmdType: _CmdType( cmd.Get<ERHICommandType::_CmdType>() ); break
				PerformCmd( SetPipelineState );
				PerformCmd( SetShaderBindingLayout );
				PerformCmd( SetRenderTargets );
				PerformCmd( ClearRenderTargets );
				PerformCmd( SetScissors );
				PerformCmd( SetViewports );
				PerformCmd( SetShaderInput );
				PerformCmd( SetIndexBuffer );
				PerformCmd( SetVertexBuffer );
				PerformCmd( SetPrimitiveTopology );
				PerformCmd( Draw );
				PerformCmd( DrawIndexed );
				PerformCmd( ResourceBarrier );
				PerformCmd( DispatchCompute );
				PerformCmd( FenceSignal );
				PerformCmd( FenceWait );
				PerformCmd( Execute );
				default: ASSERT_LOG( false, "Unknown command type!" ); break;
			}
		}

		return true;
	}

	bool D3D12CommandList::Commit( const void* a_Params )
	{
		const auto* desc = ParamsToDescriptor<RHICommandListDescriptor>( a_Params );
		if ( !desc )
		{
			return false;
		}

		D3D12RHI* rhi = RHI::GetD3D12RHI();
		if ( FAILED( rhi->GetDevice()->CreateCommandList1( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS( &CommandList ) ) ) )
		{
			return false;
		}

		return true;
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

	//////////////////////////////////////////////////////////////////////////

	void D3D12CommandList::SetPipelineState( const RHICommand::SetPipelineState& a_Data )
	{
		CommandList->SetPipelineState( a_Data.PSO->As<D3D12PipelineState>()->PSO.Get() );
	}

	void D3D12CommandList::SetShaderBindingLayout( const RHICommand::SetShaderBindingLayout& a_Data )
	{
		CurrentSBL = a_Data.SBL;
		CommandList->SetGraphicsRootSignature( a_Data.SBL->As<D3D12ShaderBindingLayout>()->m_RootSignature.Get() );
	}

	void D3D12CommandList::SetRenderTargets( const RHICommand::SetRenderTargets& a_Data )
	{
		if ( a_Data.RTV.Size() == 0 )
		{
			CommandList->OMSetRenderTargets( 0, nullptr, FALSE, nullptr );
			return;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE rtvs[RHIQuery::MaxColorTargets];
		for ( size_t i = 0; i < a_Data.RTV.Size(); ++i )
		{
			rtvs[i] = a_Data.RTV[i]->As<D3D12Texture>()->DescriptorHandle;
		}

		if ( a_Data.DSV )
		{
			D3D12_CPU_DESCRIPTOR_HANDLE dsv = a_Data.DSV->As<D3D12Texture>()->DescriptorHandle;
			CommandList->OMSetRenderTargets( a_Data.RTV.Size(), rtvs, FALSE, &dsv );
		}
		else
		{
			CommandList->OMSetRenderTargets( a_Data.RTV.Size(), rtvs, FALSE, nullptr );
		}
	}

	void D3D12CommandList::ClearRenderTargets( const RHICommand::ClearRenderTargets& a_Data )
	{
		for ( size_t i = 0; i < a_Data.RTV.Size(); ++i )
		{
			CommandList->ClearRenderTargetView( a_Data.RTV[i]->As<D3D12Texture>()->DescriptorHandle, &a_Data.ClearColor[0], 0, nullptr );
		}
	}

	void D3D12CommandList::SetScissors( const RHICommand::SetScissors& a_Data )
	{
		TODO( "I just slapped a thread_local on this as I'm not sure if D3D12 needs me to keep the data around" );
		thread_local RECT rects[RHIQuery::MaxColorTargets];
		for ( size_t i = 0; i < a_Data.Rects.Size(); ++i )
		{
			const auto& rect = a_Data.Rects[i];
			rects[i].left = rect.Left;
			rects[i].top = rect.Top;
			rects[i].right = rect.Right;
			rects[i].bottom = rect.Bottom;
		}

		CommandList->RSSetScissorRects( a_Data.Rects.Size(), rects );
	}

	void D3D12CommandList::SetViewports( const RHICommand::SetViewports& a_Data )
	{
		// Check the offsets of the Tridium Viewport and D3D12_VIEWPORT variables are the same so we can use a cast
		static_assert( sizeof( Viewport ) == sizeof( D3D12_VIEWPORT ) );
		static_assert( offsetof( Viewport, X ) == offsetof( D3D12_VIEWPORT, TopLeftX ) );
		static_assert( offsetof( Viewport, Y ) == offsetof( D3D12_VIEWPORT, TopLeftY ) );
		static_assert( offsetof( Viewport, Width ) == offsetof( D3D12_VIEWPORT, Width ) );
		static_assert( offsetof( Viewport, Height ) == offsetof( D3D12_VIEWPORT, Height ) );
		static_assert( offsetof( Viewport, MinDepth ) == offsetof( D3D12_VIEWPORT, MinDepth ) );
		static_assert( offsetof( Viewport, MaxDepth ) == offsetof( D3D12_VIEWPORT, MaxDepth ) );

		CommandList->RSSetViewports( a_Data.Viewports.Size(), reinterpret_cast<const D3D12_VIEWPORT*>( a_Data.Viewports.Data() ) );
	}

	void D3D12CommandList::SetShaderInput( const RHICommand::SetShaderInput& a_Data )
	{
		if ( !CurrentSBL )
		{
			ASSERT_LOG( false, "No Shader Binding Layout set!" );
			return;
		}

		const RHIShaderBindingLayoutDescriptor* desc = CurrentSBL->GetDescriptor();
		const int32_t index = desc->GetBindingIndex( a_Data.NameHash );
		if ( index == -1 )
		{
			ASSERT_LOG( false, "Shader Binding not found!" );
			return;
		}
		const RHIShaderBinding& binding = desc->Bindings.At( index );

		switch ( binding.BindingType )
		{
		case ERHIShaderBindingType::Constant:
		{
			if ( binding.IsInlined )
			{
				CommandList->SetGraphicsRoot32BitConstants( ShaderInputOffset++, binding.WordSize, static_cast<const void*>( &a_Data.Payload.InlineData[0] ), 0 );
			}
			else
			{
				NOT_IMPLEMENTED;
			}
			break;
		}
		case ERHIShaderBindingType::Mutable:
		{

			break;
		}
		case ERHIShaderBindingType::Storage:
		{
			break;
		}
		case ERHIShaderBindingType::Texture:
		{
			D3D12_DESCRIPTOR_HEAP_DESC dhd{};
			dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			dhd.NumDescriptors = 8;
			dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			dhd.NodeMask = 0;

			TODO( "Temp hack as srvheap needs to stay around" );
			ComPtr<ID3D12DescriptorHeap> srvheap;
			RHI::GetD3D12RHI()->GetDevice()->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&srvheap));
			DescriptorHeaps.PushBack( srvheap );

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			if ( a_Data.Payload.Count > 1 )
			{
				// Texture Array
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.MostDetailedMip = 0;
				srvDesc.Texture2DArray.MipLevels = 1;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.ArraySize = a_Data.Payload.Count;
				srvDesc.Texture2DArray.PlaneSlice = 0;
				srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;

				NOT_IMPLEMENTED;
			}
			else
			{
				// Single Texture
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.PlaneSlice = 0;
				srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

				RHI::GetD3D12RHI()->GetDevice()->CreateShaderResourceView(
					static_cast<const D3D12Texture*>( a_Data.Payload.References[0] )->Texture.Get(),
					&srvDesc,
					srvheap->GetCPUDescriptorHandleForHeapStart()
				);
			}

			CommandList->SetDescriptorHeaps( 1, &srvheap );
			CommandList->SetGraphicsRootDescriptorTable( ShaderInputOffset++, srvheap->GetGPUDescriptorHandleForHeapStart() );

			break;
		}
		case ERHIShaderBindingType::RWTexture:
		{
			break;
		}
		case ERHIShaderBindingType::Sampler:
		{
			if ( a_Data.Payload.Count > 1 )
			{
				NOT_IMPLEMENTED;
			}
			else
			{
				D3D12Sampler* sampler = static_cast<D3D12Sampler*>( a_Data.Payload.References[0] );
				CommandList->SetDescriptorHeaps( 1, &sampler->SamplerHeap );
				CommandList->SetGraphicsRootDescriptorTable( ShaderInputOffset++, sampler->SamplerHeap->GetGPUDescriptorHandleForHeapStart() );
			}
			break;
		}
		case ERHIShaderBindingType::StaticSampler:
		{
			break;
		}
		default:
		{
			ASSERT_LOG( false, "Unknown Shader Binding Type!" );
			break;
		}
		}
	}

	void D3D12CommandList::SetIndexBuffer( const RHICommand::SetIndexBuffer& a_Data )
	{
	}

	void D3D12CommandList::SetVertexBuffer( const RHICommand::SetVertexBuffer& a_Data )
	{
		D3D12_VERTEX_BUFFER_VIEW vbv{};
		vbv.BufferLocation = a_Data.VBO->As<D3D12VertexBuffer>()->VBO->GetGPUVirtualAddress();
		vbv.SizeInBytes = static_cast<UINT>( a_Data.VBO->As<D3D12VertexBuffer>()->VBO->GetDesc().Width );
		vbv.StrideInBytes = static_cast<UINT>( a_Data.VBO->GetDescriptor()->Layout.Stride );
		CommandList->IASetVertexBuffers( 0, 1, &vbv );
	}

	void D3D12CommandList::SetPrimitiveTopology( const RHICommand::SetPrimitiveTopology& a_Data )
	{
		D3D_PRIMITIVE_TOPOLOGY topology;
		switch ( a_Data.Topology )
		{
		case ERHITopology::Point:
			topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;
		case ERHITopology::Triangle:
			topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case ERHITopology::TriangleStrip:
			topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		case ERHITopology::Line:
			topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			break;
		case ERHITopology::LineStrip:
			topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			break;
		default:
			topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
			break;
		}

		CommandList->IASetPrimitiveTopology( topology );
	}

	void D3D12CommandList::Draw( const RHICommand::Draw& a_Data )
	{
		CommandList->DrawInstanced( a_Data.VertexCount, 1, a_Data.VertexStart, 0 );
	}

	void D3D12CommandList::DrawIndexed( const RHICommand::DrawIndexed& a_Data )
	{
		CommandList->DrawIndexedInstanced( a_Data.IndexCount, 1, a_Data.IndexStart, 0, 0 );
	}

	void D3D12CommandList::ResourceBarrier( const RHICommand::ResourceBarrier& a_Data )
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = a_Data.Resource->NativePtrAs<ID3D12Resource*>();
		barrier.Transition.StateBefore = ToD3D12::GetResourceState( a_Data.Before );
		barrier.Transition.StateAfter = ToD3D12::GetResourceState( a_Data.After );
		barrier.Transition.Subresource = 0;
		CommandList->ResourceBarrier( 1, &barrier );
	}

	void D3D12CommandList::DispatchCompute( const RHICommand::DispatchCompute& a_Data )
	{
	}

	void D3D12CommandList::FenceSignal( const RHICommand::FenceSignal& a_Data )
	{
	}

	void D3D12CommandList::FenceWait( const RHICommand::FenceWait& a_Data )
	{
	}

	void D3D12CommandList::Execute( const RHICommand::Execute& a_Data )
	{
	}

} // namespace Tridium