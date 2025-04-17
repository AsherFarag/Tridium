#include "tripch.h"
#include "D3D12CommandList.h"
#include "D3D12PipelineState.h"
#include "D3D12Sampler.h"
#include "D3D12Texture.h"
#include "D3D12Buffer.h"
#include "D3D12Mesh.h"
#include "D3D12DynamicRHI.h"
#include "D3D12ShaderBindingLayout.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {

	namespace RootParameters
	{
		enum
		{
			Constants = 2,
			CBV = 1,
			Textures = 0,
			Samplers = 1,
		};
	}

	D3D12CommandList::D3D12CommandList( const RHICommandListDescriptor& a_Desc )
		: RHICommandList( a_Desc )
	{
		const HRESULT hr = GetD3D12RHI()->GetDevice()->CreateCommandList1( 
			0, D3D12::Translate( m_Desc.QueueType ), D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS( &CommandList )
		);

		ENSURE_LOG( SUCCEEDED( hr ), "Failed to create command list!" );
	}

	bool D3D12CommandList::Release()
	{
		CommandList.Release();
		return true;
	}

	bool D3D12CommandList::SetGraphicsCommands( const RHIGraphicsCommandBuffer& a_CmdBuffer )
	{
		RHI_DEV_CHECK( m_Desc.QueueType == ERHICommandQueueType::Graphics, "Command list is not a graphics command list!" );

		D3D12RHI* rhi = GetD3D12RHI();
		CHECK( rhi );

		const auto& cmdAllocator = rhi->GetCommandContext( ERHICommandQueueType::Graphics ).CmdAllocator;
		// Reset the command allocator
		if ( FAILED( cmdAllocator->Reset() ) )
		{
			ASSERT_LOG( false, "Failed to reset command allocator!" );
			return false;
		}

		// Reset the command list
		if ( FAILED( GraphicsCommandList()->Reset( cmdAllocator.Get(), nullptr ) ) )
		{
			ASSERT_LOG( false, "Failed to reset command list!" );
			return false;
		}

		// Execute the commands
		for ( const RHICommand& cmd : a_CmdBuffer.Commands )
		{
			switch ( cmd.Type() )
			{
			#define PerformCmd( _CmdType ) case ERHICommandType::_CmdType: _CmdType( cmd.Get<ERHICommandType::_CmdType>() ); break
				PerformCmd( SetShaderBindingLayout );
				PerformCmd( SetShaderInput );
				PerformCmd( ResourceBarrier );
				PerformCmd( UpdateBuffer );
				PerformCmd( CopyBuffer );
				PerformCmd( UpdateTexture );
				PerformCmd( CopyTexture );
				PerformCmd( SetGraphicsPipelineState );
				PerformCmd( SetRenderTargets );
				PerformCmd( ClearRenderTargets );
				PerformCmd( SetScissors );
				PerformCmd( SetViewports );
				PerformCmd( SetIndexBuffer );
				PerformCmd( SetVertexBuffer );
				PerformCmd( SetPrimitiveTopology );
				PerformCmd( Draw );
				PerformCmd( DrawIndexed );
				PerformCmd( SetComputePipelineState );
				PerformCmd( DispatchCompute );
				PerformCmd( DispatchComputeIndirect );
				default: ASSERT_LOG( false, "Unknown command type!" ); break;
			}
		}

		m_State.Clear();

		return true;
	}

	bool D3D12CommandList::SetComputeCommands( const RHIComputeCommandBuffer& a_CmdBuffer )
	{
		RHI_DEV_CHECK( m_Desc.QueueType == ERHICommandQueueType::Compute, "Command list is not a compute command list!" );

		NOT_IMPLEMENTED;
		return false;
	}

	void D3D12CommandList::CommitBarriers()
	{
		if ( m_ResourceStateTracker.ResourceBarriers.Size() == 0 )
		{
			return;
		}

		m_State.D3D12Barriers.Clear();
		m_State.D3D12Barriers.Reserve( m_ResourceStateTracker.ResourceBarriers.Size() );

		// Commit the resource barriers
		for ( const auto& barrier : m_ResourceStateTracker.ResourceBarriers )
		{
			const D3D12_RESOURCE_STATES before = D3D12::Translate( barrier.Before );
			const D3D12_RESOURCE_STATES after = D3D12::Translate( barrier.After );
			if ( before == after )
			{
				continue;
			}

			auto& d3d12Barrier = m_State.D3D12Barriers.EmplaceBack();
			d3d12Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			d3d12Barrier.Transition.pResource = barrier.Resource->NativePtrAs<ID3D12Resource>();
			d3d12Barrier.Transition.StateBefore = before;
			d3d12Barrier.Transition.StateAfter = after;
			d3d12Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		}

		if ( m_State.D3D12Barriers.Size() > 0 )
			GraphicsCommandList()->ResourceBarrier( m_State.D3D12Barriers.Size(), m_State.D3D12Barriers.Data() );

		m_ResourceStateTracker.ResourceBarriers.Clear();
	}

	//////////////////////////////////////////////////////////////////////////

	void D3D12CommandList::SetShaderInput( const RHICommand::SetShaderInput& a_Cmd )
	{
		if ( !CurrentSBL )
		{
			ASSERT_LOG( false, "No Shader Binding Layout set!" );
			return;
		}

		const RHIShaderBindingLayoutDescriptor& desc = CurrentSBL->Descriptor();
		const int32_t index = desc.GetBindingIndex( a_Cmd.NameHash );
		if ( index == -1 )
		{
			ASSERT_LOG( false, "Shader Binding not found!" );
			return;
		}
		const RHIShaderBinding& binding = desc.Bindings.At( index );

		switch ( binding.BindingType )
		{
		case ERHIShaderBindingType::Constant:
		{
			if ( binding.IsInlined() )
			{
				GraphicsCommandList()->SetGraphicsRoot32BitConstants( RootParameters::Constants, binding.WordSize, static_cast<const void*>( &a_Cmd.Payload.InlineData[0] ), 0 );
			}
			else
			{
				NOT_IMPLEMENTED;
			}
			break;
		}
		case ERHIShaderBindingType::Mutable:
		{
			NOT_IMPLEMENTED;
			break;
		}
		case ERHIShaderBindingType::Storage:
		{
			NOT_IMPLEMENTED;
			break;
		}
		case ERHIShaderBindingType::Texture:
		{
			const D3D12::DescriptorHeapRef& srvHeap = m_State.Heaps.EmplaceBack( GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
				ERHIDescriptorHeapType::RenderResource,
				8,
				ED3D12DescriptorHeapFlags::Poolable,
				"SRV Heap" ) );

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			if ( a_Cmd.Payload.Count > 1 )
			{
				// Texture Array
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.MostDetailedMip = 0;
				srvDesc.Texture2DArray.MipLevels = 1;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.ArraySize = a_Cmd.Payload.Count;
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

				const auto* tex = static_cast<const D3D12Texture*>( a_Cmd.Payload.References[0] );

				GetD3D12RHI()->GetDevice()->CreateShaderResourceView(
					tex->Texture.Resource.Get(),
					&srvDesc,
					srvHeap->GetCPUHandle( 0 )
				);
			}

			ID3D12DescriptorHeap* d3d12Heap = srvHeap->Heap();
			GraphicsCommandList()->SetDescriptorHeaps( 1, &d3d12Heap );
			GraphicsCommandList()->SetGraphicsRootDescriptorTable( RootParameters::Textures, srvHeap->GetGPUHandle( 0 ) );

			break;
		}
		case ERHIShaderBindingType::RWTexture:
		{
			NOT_IMPLEMENTED;
			break;
		}
		case ERHIShaderBindingType::Sampler:
		{
			if ( a_Cmd.Payload.Count > 1 )
			{
				NOT_IMPLEMENTED;
			}
			else
			{
				D3D12Sampler* sampler = static_cast<D3D12Sampler*>( a_Cmd.Payload.References[0] );
				GraphicsCommandList()->SetDescriptorHeaps( 1, &sampler->SamplerHeap );
				GraphicsCommandList()->SetGraphicsRootDescriptorTable( RootParameters::Samplers, sampler->SamplerHeap->GetGPUDescriptorHandleForHeapStart() );
			}
			break;
		}
		case ERHIShaderBindingType::CombinedSampler:
		{
			// Create the SRV Heap
			D3D12::DescriptorHeapRef srvHeap = m_State.Heaps.EmplaceBack( GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
				ERHIDescriptorHeapType::RenderResource,
				8,
				ED3D12DescriptorHeapFlags::Poolable | ED3D12DescriptorHeapFlags::GPUVisible,
				"SRV Heap" ) );

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			const auto* tex = static_cast<const D3D12Texture*>( a_Cmd.Payload.References[0] );

			if ( a_Cmd.Payload.Count > 1 )
			{
				// Texture Array
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.MostDetailedMip = 0;
				srvDesc.Texture2DArray.MipLevels = 1;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.ArraySize = a_Cmd.Payload.Count;
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

				GetD3D12RHI()->GetDevice()->CreateShaderResourceView(
					tex->Texture.Resource.Get(),
					&srvDesc,
					srvHeap->GetCPUHandle( 0 )
				);

				// Retrieve the sampler from the texture
				const auto& sampler = tex->Sampler->Descriptor();

				// Create the Sampler Descriptor
				D3D12_SAMPLER_DESC samplerDesc{};
				samplerDesc.Filter = D3D12::Translate( sampler.Filter );
				samplerDesc.AddressU = D3D12::Translate( sampler.AddressU );
				samplerDesc.AddressV = D3D12::Translate( sampler.AddressV );
				samplerDesc.AddressW = D3D12::Translate( sampler.AddressW );
				samplerDesc.MipLODBias = sampler.MipLODBias;
				samplerDesc.MaxAnisotropy = sampler.MaxAnisotropy;
				samplerDesc.ComparisonFunc = D3D12::Translate( sampler.ComparisonFunc );
				samplerDesc.BorderColor[0] = sampler.BorderColor.r;
				samplerDesc.BorderColor[1] = sampler.BorderColor.g;
				samplerDesc.BorderColor[2] = sampler.BorderColor.b;
				samplerDesc.BorderColor[3] = sampler.BorderColor.a;
				samplerDesc.MinLOD = sampler.MinLOD;
				samplerDesc.MaxLOD = sampler.MaxLOD;

				// Create the Sampler Heap
				D3D12::DescriptorHeapRef samplerHeap = m_State.Heaps.EmplaceBack( GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
					ERHIDescriptorHeapType::Sampler,
					1,
					ED3D12DescriptorHeapFlags::Poolable | ED3D12DescriptorHeapFlags::GPUVisible,
					"Sampler Heap" ) );

				GetD3D12RHI()->GetDevice()->CreateSampler(
					&samplerDesc,
					samplerHeap->GetCPUHandle( 0 )
				);



				// Bind both SRV and Sampler heaps
				ID3D12DescriptorHeap* heaps[] = { srvHeap->Heap(), samplerHeap->Heap() };
				GraphicsCommandList()->SetDescriptorHeaps( 2, heaps );

				GraphicsCommandList()->SetGraphicsRootDescriptorTable( RootParameters::Textures, srvHeap->GetGPUHandle( 0 ) );
				GraphicsCommandList()->SetGraphicsRootDescriptorTable( RootParameters::Samplers, samplerHeap->GetGPUHandle( 0 ) );
			}

			break;
		}
		default:
		{
			ASSERT_LOG( false, "Unknown Shader Binding Type!" );
			break;
		}
		}
	}

	void D3D12CommandList::ResourceBarrier( const RHICommand::ResourceBarrier& a_Cmd )
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = a_Cmd.Barrier.Resource->NativePtrAs<ID3D12Resource>();
		barrier.Transition.StateBefore = D3D12::Translate( a_Cmd.Barrier.Before );
		barrier.Transition.StateAfter = D3D12::Translate( a_Cmd.Barrier.After );
		barrier.Transition.Subresource = 0;
		GraphicsCommandList()->ResourceBarrier( 1, &barrier );

		// Update the resource state
		a_Cmd.Barrier.Resource->GetType() == ERHIResourceType::Texture ?
			static_cast<D3D12Texture*>( a_Cmd.Barrier.Resource )->SetState( a_Cmd.Barrier.After ) :
			static_cast<D3D12Buffer*>( a_Cmd.Barrier.Resource )->SetState( a_Cmd.Barrier.After );
	}

	void D3D12CommandList::UpdateBuffer( const RHICommand::UpdateBuffer& a_Cmd )
	{
		D3D12Buffer* buffer = a_Cmd.Buffer->As<D3D12Buffer>();

		void* cpuVA;
		D3D12_GPU_VIRTUAL_ADDRESS gpuVA;

		TODO( "Reusing upload buffers" );
		auto& uploadBuffer = m_State.D3D12Resources.EmplaceBack();
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC uploadBufferDesc = {};
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Width = a_Cmd.Data.size();
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if ( !uploadBuffer.Commit( uploadBufferDesc, allocDesc, D3D12_RESOURCE_STATE_GENERIC_READ ) )
			return;

		// Copy data to upload buffer
		char* uploadBufferAddress;
		D3D12_RANGE uploadRange = { 0, a_Cmd.Data.size() };
		uploadBuffer.Resource->Map( 0, &uploadRange, (void**)&uploadBufferAddress );
		std::memcpy( uploadBufferAddress, a_Cmd.Data.data(), a_Cmd.Data.size() );
		uploadBuffer.Resource->Unmap( 0, &uploadRange );

		// Check the buffer state
		if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Transition )
		{
			m_ResourceStateTracker.RequireBufferState( *a_Cmd.Buffer, ERHIResourceStates::CopyDest );
		}
		else if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.Buffer->GetState() == ERHIResourceStates::CopyDest, "Buffer state is not CopyDest!" );
		}

		CommitBarriers();

		GraphicsCommandList()->CopyBufferRegion(
			buffer->ManagedBuffer.Resource.Get(),
			a_Cmd.Offset,
			uploadBuffer.Resource.Get(),
			0,
			a_Cmd.Data.size()
		);
	}

	void D3D12CommandList::CopyBuffer( const RHICommand::CopyBuffer& a_Cmd )
	{
		// Check the source buffer state
		if ( a_Cmd.SrcStateTransitionMode == ERHIResourceStateTransitionMode::Transition )
		{
			m_ResourceStateTracker.RequireBufferState( *a_Cmd.Source, ERHIResourceStates::CopySource );
		}
		else if ( a_Cmd.SrcStateTransitionMode == ERHIResourceStateTransitionMode::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.Source->GetState() == ERHIResourceStates::CopySource, "Source buffer state is not CopySource!" );
		}
		// Check the destination buffer state
		if ( a_Cmd.DstStateTransitionMode == ERHIResourceStateTransitionMode::Transition )
		{
			m_ResourceStateTracker.RequireBufferState( *a_Cmd.Destination, ERHIResourceStates::CopyDest );
		}
		else if ( a_Cmd.DstStateTransitionMode == ERHIResourceStateTransitionMode::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.Destination->GetState() == ERHIResourceStates::CopyDest, "Destination buffer state is not CopyDest!" );
		}

		CommitBarriers();

		GraphicsCommandList()->CopyBufferRegion(
			a_Cmd.Destination->As<D3D12Buffer>()->ManagedBuffer.Resource.Get(),
			a_Cmd.DestinationOffset,
			a_Cmd.Source->As<D3D12Buffer>()->ManagedBuffer.Resource.Get(),
			a_Cmd.SourceOffset,
			a_Cmd.Size
		);
	}

	void D3D12CommandList::UpdateTexture( const RHICommand::UpdateTexture& a_Cmd )
	{
		// Transition or validate resource state
		if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Transition )
		{
			m_ResourceStateTracker.RequireTextureState( *a_Cmd.Texture, ERHIResourceStates::CopyDest );
		}
		else if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.Texture->GetState() == ERHIResourceStates::CopyDest, "Texture state is not CopyDest!" );
		}

		CommitBarriers();

		// Create an upload buffer
		auto& uploadBuffer = m_State.D3D12Resources.EmplaceBack();
		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC textureDesc = a_Cmd.Texture->As<D3D12Texture>()->Texture.Resource->GetDesc();

		UINT64 requiredSize = 0;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
		UINT numRows = 0;
		UINT64 rowSizeInBytes = 0;
		UINT64 totalBytes = 0;

		GetD3D12RHI()->GetDevice()->GetCopyableFootprints(
			&textureDesc,
			a_Cmd.MipLevel,
			1,
			0,
			&footprint,
			&numRows,
			&rowSizeInBytes,
			&requiredSize
		);

		D3D12_RESOURCE_DESC uploadBufferDesc = {};
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Width = requiredSize;
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if ( !uploadBuffer.Commit( uploadBufferDesc, allocDesc, D3D12_RESOURCE_STATE_GENERIC_READ ) )
			return;

		// Copy data to upload buffer
		char* uploadBufferAddress = nullptr;
		D3D12_RANGE mapRange = { 0, static_cast<SIZE_T>( requiredSize ) };
		uploadBuffer.Resource->Map( 0, &mapRange, reinterpret_cast<void**>( &uploadBufferAddress ) );
		std::memcpy( uploadBufferAddress, a_Cmd.Data.Data.data(), a_Cmd.Data.Data.size() );
		uploadBuffer.Resource->Unmap( 0, nullptr );

		// Setup copy locations
		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
		dstLocation.pResource = a_Cmd.Texture->As<D3D12Texture>()->Texture.Resource.Get();
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex = D3D12CalcSubresource( a_Cmd.MipLevel, a_Cmd.ArraySlice, 0, textureDesc.MipLevels, textureDesc.DepthOrArraySize );

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource = uploadBuffer.Resource.Get();
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint = footprint;

		// Perform the copy
		GraphicsCommandList()->CopyTextureRegion(
			&dstLocation,
			a_Cmd.Region.MinX,
			a_Cmd.Region.MinY,
			a_Cmd.Region.MinZ,
			&srcLocation,
			nullptr // use full source footprint
		);
	}


	void D3D12CommandList::CopyTexture( const RHICommand::CopyTexture& a_Cmd )
	{
		// Check the source texture state
		if ( a_Cmd.SrcStateTransitionMode == ERHIResourceStateTransitionMode::Transition )
		{
			m_ResourceStateTracker.RequireTextureState( *a_Cmd.SrcTexture, ERHIResourceStates::CopySource );
		}
		else if ( a_Cmd.SrcStateTransitionMode == ERHIResourceStateTransitionMode::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.SrcTexture->GetState() == ERHIResourceStates::CopySource, "Source texture state is not CopySource!" );
		}
		// Check the destination texture state
		if ( a_Cmd.DstStateTransitionMode == ERHIResourceStateTransitionMode::Transition )
		{
			m_ResourceStateTracker.RequireTextureState( *a_Cmd.DstTexture, ERHIResourceStates::CopyDest );
		}
		else if ( a_Cmd.DstStateTransitionMode == ERHIResourceStateTransitionMode::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.DstTexture->GetState() == ERHIResourceStates::CopyDest, "Destination texture state is not CopyDest!" );
		}

		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
		dstLocation.pResource = a_Cmd.DstTexture->As<D3D12Texture>()->Texture.Resource.Get();
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex = D3D12CalcSubresource( 
			a_Cmd.DstRegion.MinZ, a_Cmd.DstRegion.MinY, a_Cmd.DstRegion.MinX,
			a_Cmd.DstTexture->Descriptor().Mips, a_Cmd.DstTexture->Descriptor().Depth);

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource = a_Cmd.SrcTexture->As<D3D12Texture>()->Texture.Resource.Get();
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		srcLocation.SubresourceIndex = D3D12CalcSubresource( 
			a_Cmd.SrcRegion.MinZ, a_Cmd.SrcRegion.MinY, a_Cmd.SrcRegion.MinX,
			a_Cmd.SrcTexture->Descriptor().Mips, a_Cmd.SrcTexture->Descriptor().Depth );

		GraphicsCommandList()->CopyTextureRegion(
			&dstLocation,
			a_Cmd.DstRegion.MinX,
			a_Cmd.DstRegion.MinY,
			a_Cmd.DstRegion.MinZ,
			&srcLocation,
			nullptr // use full source footprint
		);
	}

	void D3D12CommandList::SetGraphicsPipelineState( const RHICommand::SetGraphicsPipelineState& a_Cmd )
	{
		m_State.Graphics.PSO = SharedPtrCast<D3D12GraphicsPipelineState>( a_Cmd.PSO->shared_from_this() );
		GraphicsCommandList()->SetPipelineState( m_State.Graphics.PSO->PSO.Get() );
	}

	void D3D12CommandList::SetShaderBindingLayout( const RHICommand::SetShaderBindingLayout& a_Cmd )
	{
		CurrentSBL = a_Cmd.SBL;
		GraphicsCommandList()->SetGraphicsRootSignature( a_Cmd.SBL->As<D3D12ShaderBindingLayout>()->m_RootSignature.Get() );
	}

	void D3D12CommandList::SetRenderTargets( const RHICommand::SetRenderTargets& a_Cmd )
	{
		if ( a_Cmd.RTV.Size() == 0 )
		{
			TODO( "Dodgy, bad logic" );
			GraphicsCommandList()->OMSetRenderTargets( 0, nullptr, FALSE, nullptr );
			m_State.LastRTVHeap = nullptr;
			m_State.LastDSVHeap = nullptr;
			return;
		}

		if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Transition )
		{
			// Set the render target state for all RTVs and DSVs
			for ( RHITexture* rtv : a_Cmd.RTV )
			{
				m_ResourceStateTracker.RequireTextureState( *rtv, ERHIResourceStates::RenderTarget );
			}
			if ( a_Cmd.DSV )
			{
				m_ResourceStateTracker.RequireTextureState( *a_Cmd.DSV, ERHIResourceStates::DepthStencilWrite );
			}
		}
		else if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Validate )
		{
			// Check the render target state for all RTVs and DSVs
			for ( RHITexture* rtv : a_Cmd.RTV )
			{
				RHI_DEV_CHECK( rtv->GetState() == ERHIResourceStates::RenderTarget, "Render target state is not RenderTarget!" );
			}
			if ( a_Cmd.DSV )
			{
				RHI_DEV_CHECK( a_Cmd.DSV->GetState() == ERHIResourceStates::DepthStencilWrite, "Depth stencil state is not DepthStencilWrite!" );
			}
		}

		CommitBarriers();

		// Set the render targets
		m_State.Graphics.CurrentRTs.Clear();
		for ( RHITexture* rtv : a_Cmd.RTV )
		{
			m_State.Graphics.CurrentRTs.EmplaceBack( rtv );
		}
		m_State.Graphics.CurrentDSV = a_Cmd.DSV;

		const auto& device = GetD3D12RHI()->GetDevice();
		m_State.LastRTVHeap = m_State.Heaps.EmplaceBack( GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
			ERHIDescriptorHeapType::RenderTarget,
			a_Cmd.RTV.Size(),
			ED3D12DescriptorHeapFlags::Poolable,
			"RTV Heap" ) );

		// Add the RTVs to the rtv heap
		D3D12_CPU_DESCRIPTOR_HANDLE rtvs[RHIConstants::MaxColorTargets];
		for ( size_t i = 0; i < a_Cmd.RTV.Size(); ++i )
		{
			device->CreateRenderTargetView( a_Cmd.RTV[i]->As<D3D12Texture>()->Texture.Resource.Get(), nullptr, m_State.LastRTVHeap->GetCPUHandle( i ) );
			rtvs[i] = m_State.LastRTVHeap->GetCPUHandle( i );
		}

		if ( a_Cmd.DSV )
		{
			m_State.LastDSVHeap = m_State.Heaps.EmplaceBack( GetD3D12RHI()->GetDescriptorHeapManager().AllocateHeap(
				ERHIDescriptorHeapType::DepthStencil,
				1,
				ED3D12DescriptorHeapFlags::Poolable,
				"DSV Heap" ) );

			device->CreateDepthStencilView( a_Cmd.DSV->As<D3D12Texture>()->Texture.Resource.Get(), nullptr, m_State.LastDSVHeap->GetCPUHandle( 0 ) );
			D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_State.LastDSVHeap->GetCPUHandle( 0 );
			GraphicsCommandList()->OMSetRenderTargets( a_Cmd.RTV.Size(), rtvs, false, &dsv );
		}
		else
		{
			GraphicsCommandList()->OMSetRenderTargets( a_Cmd.RTV.Size(), rtvs, false, nullptr );
			m_State.LastDSVHeap = nullptr;
		}
	}

	void D3D12CommandList::ClearRenderTargets( const RHICommand::ClearRenderTargets& a_Cmd )
	{
		if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Transition )
		{
			// Set the render target state for all RTVs and DSVs
			for ( RHITexture* rtv : m_State.Graphics.CurrentRTs )
			{
				m_ResourceStateTracker.RequireTextureState( *rtv, ERHIResourceStates::RenderTarget );
			}
			if ( m_State.Graphics.CurrentDSV )
			{
				m_ResourceStateTracker.RequireTextureState( *m_State.Graphics.CurrentDSV, ERHIResourceStates::DepthStencilWrite );
			}
		}
		else if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Validate )
		{
			// Check the render target state for all RTVs and DSVs
			for ( RHITexture* rtv : m_State.Graphics.CurrentRTs )
			{
				RHI_DEV_CHECK( rtv->GetState() == ERHIResourceStates::RenderTarget, "Render target state is not RenderTarget!" );
			}
			if ( m_State.Graphics.CurrentDSV )
			{
				RHI_DEV_CHECK( m_State.Graphics.CurrentDSV->GetState() == ERHIResourceStates::DepthStencilWrite, "Depth stencil state is not DepthStencilWrite!" );
			}
		}

		CommitBarriers();

		if ( a_Cmd.ClearFlags.HasFlag( ERHIClearFlags::Color ) && m_State.LastRTVHeap )
		{
			for ( size_t i = 0; i < m_State.LastRTVHeap->NumDescriptors(); ++i )
			{
				GraphicsCommandList()->ClearRenderTargetView(
					m_State.LastRTVHeap->GetCPUHandle( i ),
					&a_Cmd.ClearColorValues[i].r, 0, nullptr );
			}
		}

		// Clear the depth stencil view

		D3D12_CLEAR_FLAGS clearFlags = D3D12::Translate( a_Cmd.ClearFlags );
		if ( clearFlags != 0 && m_State.LastDSVHeap )
		{
			GraphicsCommandList()->ClearDepthStencilView(
				m_State.LastDSVHeap->GetCPUHandle( 0 ),
				clearFlags,
				a_Cmd.DepthValue, a_Cmd.StencilValue,
				0, nullptr );
		}
	}

	void D3D12CommandList::SetScissors( const RHICommand::SetScissors& a_Cmd )
	{
		TODO( "I just slapped a thread_local on this as I'm not sure if D3D12 needs me to keep the data around" );
		thread_local RECT rects[RHIConstants::MaxColorTargets];
		for ( size_t i = 0; i < a_Cmd.Rects.Size(); ++i )
		{
			const auto& rect = a_Cmd.Rects[i];
			rects[i].left = rect.Left;
			rects[i].top = rect.Top;
			rects[i].right = rect.Right;
			rects[i].bottom = rect.Bottom;
		}

		GraphicsCommandList()->RSSetScissorRects( a_Cmd.Rects.Size(), rects );
	}

	void D3D12CommandList::SetViewports( const RHICommand::SetViewports& a_Cmd )
	{
		// Check the offsets of the Tridium RHIViewport and D3D12_VIEWPORT variables are the same so we can use a cast
		static_assert( sizeof( RHIViewport ) == sizeof( D3D12_VIEWPORT ) );
		static_assert( offsetof( RHIViewport, X ) == offsetof( D3D12_VIEWPORT, TopLeftX ) );
		static_assert( offsetof( RHIViewport, Y ) == offsetof( D3D12_VIEWPORT, TopLeftY ) );
		static_assert( offsetof( RHIViewport, Width ) == offsetof( D3D12_VIEWPORT, Width ) );
		static_assert( offsetof( RHIViewport, Height ) == offsetof( D3D12_VIEWPORT, Height ) );
		static_assert( offsetof( RHIViewport, MinDepth ) == offsetof( D3D12_VIEWPORT, MinDepth ) );
		static_assert( offsetof( RHIViewport, MaxDepth ) == offsetof( D3D12_VIEWPORT, MaxDepth ) );

		GraphicsCommandList()->RSSetViewports( a_Cmd.Viewports.Size(), reinterpret_cast<const D3D12_VIEWPORT*>( a_Cmd.Viewports.Data() ) );
	}

	void D3D12CommandList::SetIndexBuffer( const RHICommand::SetIndexBuffer& a_Cmd )
	{
		if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Transition )
		{
			m_ResourceStateTracker.RequireBufferState( *a_Cmd.IBO, ERHIResourceStates::IndexBuffer );
		}
		else if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.IBO->GetState() == ERHIResourceStates::IndexBuffer, "Index buffer state is not IndexBuffer!" );
		}

		CommitBarriers();

		D3D12_INDEX_BUFFER_VIEW ibv{};
		ibv.BufferLocation = a_Cmd.IBO->As<D3D12Buffer>()->ManagedBuffer.Resource->GetGPUVirtualAddress();
		ibv.SizeInBytes = static_cast<UINT>( a_Cmd.IBO->As<D3D12Buffer>()->ManagedBuffer.Resource->GetDesc().Width );
		ibv.Format = D3D12::Translate( a_Cmd.IBO->Descriptor().Format );
		GraphicsCommandList()->IASetIndexBuffer( &ibv );
	}

	void D3D12CommandList::SetVertexBuffer( const RHICommand::SetVertexBuffer& a_Cmd )
	{
		if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Transition )
		{
			m_ResourceStateTracker.RequireBufferState( *a_Cmd.VBO, ERHIResourceStates::VertexBuffer );
		}
		else if ( a_Cmd.StateTransitionMode == ERHIResourceStateTransitionMode::Validate )
		{
			RHI_DEV_CHECK( a_Cmd.VBO->GetState() == ERHIResourceStates::VertexBuffer, "Vertex buffer state is not VertexBuffer!" );
		}

		CommitBarriers();

		D3D12_VERTEX_BUFFER_VIEW vbv{};
		vbv.BufferLocation = a_Cmd.VBO->As<D3D12Buffer>()->ManagedBuffer.Resource->GetGPUVirtualAddress();
		vbv.SizeInBytes = static_cast<UINT>( a_Cmd.VBO->As<D3D12Buffer>()->ManagedBuffer.Resource->GetDesc().Width );
		vbv.StrideInBytes = m_State.Graphics.PSO->Descriptor().VertexLayout.Stride;
		GraphicsCommandList()->IASetVertexBuffers( 0, 1, &vbv );
	}

	void D3D12CommandList::SetPrimitiveTopology( const RHICommand::SetPrimitiveTopology& a_Cmd )
	{
		GraphicsCommandList()->IASetPrimitiveTopology( D3D12::Translate( a_Cmd.Topology ) );
	}

	void D3D12CommandList::Draw( const RHICommand::Draw& a_Cmd )
	{
		GraphicsCommandList()->DrawInstanced( a_Cmd.VertexCount, 1, a_Cmd.VertexStart, 0 );
	}

	void D3D12CommandList::DrawIndexed( const RHICommand::DrawIndexed& a_Cmd )
	{
		GraphicsCommandList()->DrawIndexedInstanced( a_Cmd.IndexCount, 1, a_Cmd.IndexStart, 0, 0 );
	}

	void D3D12CommandList::SetComputePipelineState( const RHICommand::SetComputePipelineState& a_Cmd )
	{
	}

	void D3D12CommandList::DispatchCompute( const RHICommand::DispatchCompute& a_Cmd )
	{
	}

	void D3D12CommandList::DispatchComputeIndirect( const RHICommand::DispatchComputeIndirect& a_Cmd )
	{
	}

	bool D3D12CommandList::IsCompleted() const
	{
		return GetD3D12RHI()->GetCommandContext( m_Desc.QueueType ).IsFenceComplete( FenceValue() );
	}

	void D3D12CommandList::WaitUntilCompleted()
	{
		GetD3D12RHI()->GetCommandContext( m_Desc.QueueType ).Wait( FenceValue() );
	}

} // namespace Tridium