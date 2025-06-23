#include "tripch.h"
#include "RHI_D3D12Impl.h"

// For getting the native window handle
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3native.h>


// Resources
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIVertexLayout.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>

namespace Tridium::D3D12 {

	//////////////////////////////////////////////////////////////////////////
	// CORE RHI FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

    bool DynamicRHI_D3D12Impl::Init( const RHIConfig& a_Config )
    {
		m_Config = a_Config;

	#if RHI_DEBUG_ENABLED
		if ( a_Config.UseDebug )
		{
			// Init D3D12 Debug layer
			if ( FAILED( D3D12GetDebugInterface( IID_PPV_ARGS( m_D3D12Debug.GetAddressOf() ) ) ) )
			{
				LOG( LogCategory::DirectX, Error, "Failed to create D3D12 debug interface!" );
				return false;
			}

			m_D3D12Debug->EnableDebugLayer();

			// Init DXGI Debug
			if ( FAILED( DXGIGetDebugInterface1( 0, IID_PPV_ARGS( m_DXGIDebug.GetAddressOf() ) ) ) )
			{
				LOG( LogCategory::DirectX, Error, "Failed to create DXGI debug interface!" );
				return false;
			}

			m_DXGIDebug->EnableLeakTrackingForThread();
		}
	#endif // RHI_DEBUG_ENABLED

		m_CmdListsToExecute.Reserve( 64 );

		// Create the DXGIFactory
        if ( FAILED( CreateDXGIFactory2( 0, IID_PPV_ARGS( m_DXGIFactory.GetAddressOf() ) ) ) )
        {
			LOG( LogCategory::DirectX, Error, "Failed to create DXGIFactory!" );
            return false;
        }

		// Create the device
        if ( FAILED( D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( m_Device.GetAddressOf() ) ) ) )
        {
			LOG( LogCategory::DirectX, Error, "Failed to create D3D12 device!" );
            return false;
        }

		// Get the max supported D3D12 device version
#define QUERY_D3D12_DEVICE_VERSION( _Version ) \
		if ( ID3D12Device##_Version* deviceVersion = nullptr; SUCCEEDED( m_Device->QueryInterface<ID3D12Device##_Version>( &deviceVersion ) ) ) \
		{ \
			m_MaxD3D12DeviceVersion = _Version; \
		}

		QUERY_D3D12_DEVICE_VERSION( 1 );
		QUERY_D3D12_DEVICE_VERSION( 2 );
		QUERY_D3D12_DEVICE_VERSION( 3 );
		QUERY_D3D12_DEVICE_VERSION( 4 );
		QUERY_D3D12_DEVICE_VERSION( 5 );

#undef QUERY_D3D12_DEVICE_VERSION

		// Retrieve the adapter
		if ( FAILED( m_DXGIFactory->EnumAdapters( 0, &m_DXGIAdapter ) ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to enumerate adapters!" );
			return false;
		}

		// Initialise GPU Info
		GPUInfo gpuInfo{};

		// Create fence event
		m_FenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );

		// Set up Command Contexts
		for ( size_t i = 0; i < m_CmdQueues.Size(); ++i )
		{
			// Set up the command context and set the command queue type
			ERHICommandQueueType cmdQueueType = Cast<ERHICommandQueueType>( i );
			D3D12_COMMAND_LIST_TYPE d3d12CmdListType;
			switch ( cmdQueueType )
			{
				case ERHICommandQueueType::Graphics:  d3d12CmdListType = D3D12_COMMAND_LIST_TYPE_DIRECT;  break;
				case ERHICommandQueueType::Compute: d3d12CmdListType = D3D12_COMMAND_LIST_TYPE_COMPUTE; break;
				case ERHICommandQueueType::Copy:    d3d12CmdListType = D3D12_COMMAND_LIST_TYPE_COPY;    break;
				default: ASSERT( false, "Invalid command queue type!" ); break;
			}

			// Create the command queue
			ComPtr<ID3D12CommandQueue> cmdQueue = nullptr;
			{
				D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
				cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
				cmdQueueDesc.NodeMask = 0;
				cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				cmdQueueDesc.Type = d3d12CmdListType;

				if ( FAILED( m_Device->CreateCommandQueue( &cmdQueueDesc, IID_PPV_ARGS( cmdQueue.GetAddressOf() ) ) ) )
					return false;

				WStringView cmdQueueName{};
				switch ( d3d12CmdListType )
				{
				case D3D12_COMMAND_LIST_TYPE_DIRECT:  cmdQueueName = L"Graphics Command Queue"; break;
				case D3D12_COMMAND_LIST_TYPE_COMPUTE: cmdQueueName = L"Compute Command Queue"; break;
				case D3D12_COMMAND_LIST_TYPE_COPY:    cmdQueueName = L"Copy Command Queue"; break;
				}

				cmdQueue->SetName( cmdQueueName.data() );
			}
			m_CmdQueues[i] = MakeUnique<CommandQueue>( *m_Device.Get(), cmdQueue.Get() );
		}

		// Create the Memory Allocator
		using enum D3D12MA::ALLOCATOR_FLAGS;
		int allocatorFlags = D3D12MA::ALLOCATOR_FLAG_NONE;
		allocatorFlags |= ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED;
		allocatorFlags |= ALLOCATOR_FLAG_MSAA_TEXTURES_ALWAYS_COMMITTED;
		allocatorFlags |= !RHI::SupportsMultithreading() ? ALLOCATOR_FLAG_SINGLETHREADED : ALLOCATOR_FLAG_NONE;
		D3D12MA::ALLOCATOR_DESC  allocatorDesc = {
			.Flags = D3D12MA::ALLOCATOR_FLAGS( allocatorFlags ),
			.pDevice = m_Device.Get(),
			.PreferredBlockSize = 0,
			.pAllocationCallbacks = nullptr,
			.pAdapter = m_DXGIAdapter.Get()
		};
		if ( FAILED( D3D12MA::CreateAllocator( &allocatorDesc, m_Allocator.GetAddressOf() ) ) )
		{
			return false;
		}

		// Init the descriptor heap manager
		m_DescriptorHeapManager.Init( m_Device.Get(), DescriptorHeapManagerDesc{} );

		// Create the upload buffer
		if ( !m_UploadBuffer.Commit( 1024 * 1024 * 64, *m_Allocator.Get() ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to create upload buffer" );
			return false;
		}

		// Create resource initializer command list
		{
			auto resourceInitCmdListDesc = RHICommandListDesc{}
				.SetQueueType( ERHICommandQueueType::Copy )
				.SetEnableImmediateExecution( false )
				.SetName( "ResourceInitCmdList" );

			m_ResourceInitCmdList = CreateCommandList( resourceInitCmdListDesc );
		}

		// Create the swap chain
		m_SwapChain = CreateSwapChain( a_Config.SwapChainDesc );

        return true;
    }

	bool DynamicRHI_D3D12Impl::Shutdown()
	{
		WaitForIdle();

		m_DescriptorHeapManager.Shutdown();

		if ( m_SwapChain )
		{
			m_SwapChain->Release();
			m_SwapChain.reset();
		}

		LOG( LogCategory::RHI, Info, "Releasing all registered resources...", m_RegisteredResources.size() );
		size_t numResources = 0;
		for ( const auto& [hash, resourceWeakRef] : m_RegisteredResources )
		{
			if ( RHIObjectRef resource = resourceWeakRef.lock() )
			{
				numResources++;
				resource->Release();
			}
		}
		LOG( LogCategory::RHI, Info, "Released {0} resources", numResources );

		m_UploadBuffer.Release();
		m_Allocator.Reset();

		for ( auto& cmdQueue : m_CmdQueues )
		{
			cmdQueue.reset();
		}

		if ( m_FenceEvent )
		{
			CloseHandle( m_FenceEvent );
			m_FenceEvent = nullptr;
		}

		if ( ULONG refCount = ForceDeleteIUnknown( m_Device.GetAddressOf() ) )
		{
			LOG( LogCategory::DirectX, Warn, "D3D12 device still has {0} references! - Destroying the device anyway", refCount );
		}

		m_DXGIAdapter.Reset();
		m_DXGIFactory.Reset();

#if RHI_DEBUG_ENABLED
		DumpDebug();
		m_DXGIDebug.Reset();
#endif

		return true;
	}

	RHIFenceValue DynamicRHI_D3D12Impl::ExecuteCommandLists( Span<IRHICommandList* const> a_CommandLists, ERHICommandQueueType a_QueueType )
	{
		m_CmdListsToExecute.Resize( a_CommandLists.size() );
		for ( size_t i = 0; i < a_CommandLists.size(); ++i )
			m_CmdListsToExecute[i] = a_CommandLists[i]->As<RHICommandList_D3D12Impl>()->GetD3D12CmdList();

		CommandQueue* cmdQueue = GetCommandQueue( a_QueueType );
		RHI_DEV_CHECK( cmdQueue, "Invalid command queue type!" );

		cmdQueue->CmdQueue->ExecuteCommandLists( Cast<UINT>( a_CommandLists.size() ), m_CmdListsToExecute.Data() );
		cmdQueue->LastSubmittedValue++;
		cmdQueue->CmdQueue->Signal( cmdQueue->Fence.Get(), cmdQueue->LastSubmittedValue );

		for ( size_t i = 0; i < a_CommandLists.size(); ++i )
		{
			UniquePtr<CommandContext> cmdCtx = a_CommandLists[i]->As<RHICommandList_D3D12Impl>()->ReleaseCmdContext( *cmdQueue );
			cmdQueue->CmdContextsInFlight.emplace_front( std::move( cmdCtx ) );
		}

		if ( FAILED( m_Device->GetDeviceRemovedReason() ) )
			LOG( LogCategory::RHI, Error, "Device removed!" );

		return cmdQueue->LastSubmittedValue;
	}

	bool DynamicRHI_D3D12Impl::WaitForIdle()
	{
		for ( const auto& queue : m_CmdQueues )
		{
			if ( queue == nullptr )
				continue;

			// Test if the fence has been reached
			if ( queue->UpdateLastCompletedValue() < queue->LastSubmittedValue )
			{
				// If it's not, wait for it to finish using an event
				ResetEvent( m_FenceEvent );
				queue->Fence->SetEventOnCompletion( queue->LastSubmittedValue, m_FenceEvent );
				WaitForSingleObject( m_FenceEvent, INFINITE );
			}
		}

		return true;
	}

	void DynamicRHI_D3D12Impl::WaitForFence( ERHICommandQueueType a_QueueType, RHIFenceValue a_FenceValue )
	{
		CommandQueue* queue = GetCommandQueue( a_QueueType );
		RHI_DEV_CHECK( queue, "Invalid command queue type!" );
		RHI_DEV_CHECK( a_FenceValue <= queue->LastSubmittedValue, "Invalid fence value!" );

		// Test if the fence has been reached
		if ( queue->UpdateLastCompletedValue() < a_FenceValue )
		{
			// If it's not, wait for it to finish using an event
			ResetEvent( m_FenceEvent );
			queue->Fence->SetEventOnCompletion( a_FenceValue, m_FenceEvent );
			WaitForSingleObject( m_FenceEvent, INFINITE );
		}
	}

	void DynamicRHI_D3D12Impl::CollectGarbage()
	{
		// Iterate through each command queue and remove completed command contexts
		for ( const UniquePtr<CommandQueue>& queue : m_CmdQueues )
		{
			if ( queue == nullptr )
				continue;

			queue->UpdateLastCompletedValue();

			while ( !queue->CmdContextsInFlight.empty() 
				  && queue->CmdContextsInFlight.back()->SubmittedValue <= queue->LastSubmittedValue )
			{
				queue->CmdContextsInFlight.pop_back();
			}
		}
	}

    //////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHITextureRef DynamicRHI_D3D12Impl::CreateTexture( const RHITextureDesc& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
	{
		RHITextureRef texture = IRHIObject::Create<RHITexture_D3D12Impl>( this, a_Desc, a_SubResourcesData );
		RegisterRHIObject( *texture.get() );
		return texture;
	}

	RHIBufferRef DynamicRHI_D3D12Impl::CreateBuffer( const RHIBufferDesc& a_Desc, Span<const uint8_t> a_Data )
	{
		RHIBufferRef buffer = IRHIObject::Create<RHIBuffer_D3D12Impl>( this, a_Desc, a_Data );
		RegisterRHIObject( *buffer.get() );
		return buffer;
	}

	RHIGraphicsPipelineStateRef DynamicRHI_D3D12Impl::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDesc& a_Desc )
	{
		SharedPtr<RootSignature> rootSig = GetRootSignature( a_Desc.BindingLayouts, a_Desc.VertexLayout.Valid() );
 		RHIGraphicsPipelineStateRef pso = IRHIObject::Create<RHIGraphicsPipelineState_D3D12Impl>( this, a_Desc, rootSig );
		RegisterRHIObject( *pso.get() );
		return pso;
	}

	RHICommandListRef DynamicRHI_D3D12Impl::CreateCommandList( const RHICommandListDesc& a_Desc )
	{
 		RHICommandListRef cmdList = IRHIObject::Create<RHICommandList_D3D12Impl>( this, a_Desc );
		RegisterRHIObject( *cmdList.get() );
		return cmdList;
	}

	RHIShaderModuleRef DynamicRHI_D3D12Impl::CreateShaderModule( const RHIShaderModuleDesc& a_Desc )
	{
		RHIShaderModuleRef shaderModule = IRHIObject::Create<RHIShaderModule_D3D12Impl>( this, a_Desc );
		RegisterRHIObject( *shaderModule.get() );
		return shaderModule;
	}

	RHIBindingLayoutRef DynamicRHI_D3D12Impl::CreateBindingLayout( const RHIBindingLayoutDesc& a_Desc )
	{
		RHIBindingLayoutRef bindingLayout = IRHIObject::Create<RHIBindingLayout_D3D12Impl>( this, a_Desc );
		RegisterRHIObject( *bindingLayout.get() );
		return bindingLayout;
	}

	RHIBindingSetRef DynamicRHI_D3D12Impl::CreateBindingSet( const RHIBindingSetDesc& a_Desc )
	{
		RHIBindingSetRef bindingSet = IRHIObject::Create<RHIBindingSet_D3D12Impl>( this, a_Desc );
		RegisterRHIObject( *bindingSet.get() );
		return bindingSet;
	}

	RHISwapChainRef DynamicRHI_D3D12Impl::CreateSwapChain( const RHISwapChainDesc& a_Desc )
	{
		RHISwapChainRef swapChain = IRHIObject::Create<RHISwapChain_D3D12Impl>( this, a_Desc );
		RegisterRHIObject( *swapChain.get() );
		return swapChain;
	}

	GPUInfo DynamicRHI_D3D12Impl::GetGPUInfo() const
	{
		GPUInfo gpuInfo{};
		if ( m_DXGIAdapter )
		{
			DXGI_ADAPTER_DESC desc{};
			m_DXGIAdapter->GetDesc( &desc );
			gpuInfo.VendorID = desc.VendorId;
			gpuInfo.DriverVersion = ToString( desc.Revision );
			gpuInfo.DeviceName = ToString( WStringView( desc.Description ) );
			gpuInfo.VRAMBytes = desc.DedicatedVideoMemory;
		}

		
		D3D_SHADER_MODEL highestShaderModel = D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_0;
		if ( D3D12_FEATURE_DATA_SHADER_MODEL shaderModel{}; 
			SUCCEEDED( m_Device->CheckFeatureSupport( D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof( shaderModel ) ) ) )
		{
			highestShaderModel = shaderModel.HighestShaderModel;
			gpuInfo.DeviceFeatures.HighestShaderModel = Translate( highestShaderModel );
		}

		// Check for feature support

		auto InitFeature = [&gpuInfo]( ERHIFeature a_Feature, bool a_Support )
			{
				gpuInfo.DeviceFeatures.Features[Cast<uint32_t>( a_Feature )].SetSupport( a_Support ? ERHIFeatureSupport::Supported : ERHIFeatureSupport::Unsupported );
			};

		// Compute shaders are always supported in D3D12
		InitFeature( ERHIFeature::ComputeShaders, highestShaderModel >= D3D_SHADER_MODEL::D3D_SHADER_MODEL_5_1 );

		
		if ( D3D12_FEATURE_DATA_D3D12_OPTIONS options{};
			SUCCEEDED( m_Device->CheckFeatureSupport( D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof( options ) ) ) )
		{
			// From: https://microsoft.github.io/DirectX-Specs/d3d/HLSL_SM_6_6_DynamicResources.html
			InitFeature( ERHIFeature::BindlessResources, 
				highestShaderModel >= D3D_SHADER_MODEL::D3D_SHADER_MODEL_6_6
				&& options.ResourceBindingTier >= D3D12_RESOURCE_BINDING_TIER_3 );
		}

		if ( D3D12_FEATURE_DATA_D3D12_OPTIONS5 options{}; 
			SUCCEEDED( m_Device->CheckFeatureSupport( D3D12_FEATURE_D3D12_OPTIONS5, &options, sizeof( options ) ) ) )
		{
			InitFeature( ERHIFeature::RayTracing, options.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED );
		}

		if ( D3D12_FEATURE_DATA_D3D12_OPTIONS7 options{};
			SUCCEEDED( m_Device->CheckFeatureSupport( D3D12_FEATURE_D3D12_OPTIONS7, &options, sizeof( options ) ) ) )
		{
			InitFeature( ERHIFeature::MeshShaders, options.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED );
		}
		

		return gpuInfo;
	}

	SharedPtr<RootSignature> DynamicRHI_D3D12Impl::GetRootSignature( Span<const RHIBindingLayoutRef> a_BindingLayouts, bool a_AllowInputLayout )
	{
		hash64_t hash = 0;
		for ( const auto& bindingLayout : a_BindingLayouts )
			hash = Hashing::HashCombine( hash, bindingLayout.get() );

		hash = Hashing::HashCombine( hash, a_AllowInputLayout );

		// Check if the root signature is already cached
		if ( auto it = m_RootSignatureCache.find( hash );
			it != m_RootSignatureCache.end() && !it->second.expired() )
		{
			return it->second.lock();
		}

		// Create a new root signature
		SharedPtr<RootSignature> newRootSignature{};
		if ( RootSignature rootSig = RootSignature::Build( a_BindingLayouts, a_AllowInputLayout, false );
			rootSig.Valid() )
		{
			rootSig.Hash = hash;
			newRootSignature = MakeShared<RootSignature>( std::move( rootSig ) );
			m_RootSignatureCache[hash] = newRootSignature;
		}
		
		return newRootSignature;
	}

	//////////////////////////////////////////////////////////////////////////
    // DEBUG
	//////////////////////////////////////////////////////////////////////////

#if RHI_DEBUG_ENABLED
	void DynamicRHI_D3D12Impl::DumpDebug()
    {
        if ( m_DXGIDebug )
        {
            OutputDebugStringW( L"DirectX12 Debug Dump:\n" );
            m_DXGIDebug->ReportLiveObjects(
                DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL
            );
			OutputDebugStringW( L"End of DirectX12 Debug Dump\n" );
        }
    }
#endif // RHI_DEBUG_ENABLED

} // namespace Tridium
