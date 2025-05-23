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
	#endif

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

		// Set up Command Contexts
		for ( size_t i = 0; i < m_CmdContexts.Size(); ++i )
		{
			// Set up the command context and set the command queue type
			CommandContext& cmdCtx = m_CmdContexts[i];

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
			{
				D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
				cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
				cmdQueueDesc.NodeMask = 0;
				cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				cmdQueueDesc.Type = d3d12CmdListType;

				if ( FAILED( m_Device->CreateCommandQueue( &cmdQueueDesc, IID_PPV_ARGS( cmdCtx.CmdQueue.GetAddressOf() ) ) ) )
				{
					return false;
				}
			}

			// Create the command allocator
			if ( FAILED( m_Device->CreateCommandAllocator( d3d12CmdListType, IID_PPV_ARGS( cmdCtx.CmdAllocator.GetAddressOf() ) ) ) )
			{
				return false;
			}

			// Create the command list
			if ( FAILED( GetD3D12Device4()->CreateCommandList1( 0, d3d12CmdListType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS( cmdCtx.CmdList.GetAddressOf() ) ) ) )
			{
				return false;
			}

			// Create the fence
			cmdCtx.FenceValue = 0;
			cmdCtx.FenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
			if ( cmdCtx.FenceEvent == nullptr )
			{
				return false;
			}
			if ( FAILED( m_Device->CreateFence( cmdCtx.FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( cmdCtx.Fence.GetAddressOf() ) ) ) )
			{
				return false;
			}

			TODO( "Temp" );
			cmdCtx.CmdQueue->SetName( L"CmdCtx-RHICommandQueue" );
			cmdCtx.CmdAllocator->SetName( L"CmdCtx-RHICommandAllocator" );
			cmdCtx.Fence->SetName( L"CmdCtx-RHICommandFence" );
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
		allocatorDesc.pDevice = m_Device.Get();
		allocatorDesc.pAdapter = m_DXGIAdapter.Get();
		allocatorDesc.PreferredBlockSize = 0;
		if ( FAILED( D3D12MA::CreateAllocator( &allocatorDesc, m_Allocator.GetAddressOf() ) ) )
		{
			return false;
		}

		// Init the descriptor heap manager
		TODO( "Figure out some better values/where to get them from" );
		const uint32_t numResourceDescriptors = 2048;
		const uint32_t numSamplerDescriptors = 512;
		m_DescriptorHeapManager.Init( m_Device.Get(), numResourceDescriptors, numSamplerDescriptors );

		// Create the upload buffer
		if ( !m_UploadBuffer.Commit( 1024 * 1024 * 64, *m_Allocator.Get() ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to create upload buffer" );
			return false;
		}


        return true;
    }

	bool DynamicRHI_D3D12Impl::Shutdown()
	{
		for ( auto& cmdCtx : m_CmdContexts )
		{
			cmdCtx.Wait( cmdCtx.Signal() );
			cmdCtx.~CommandContext();
		}
		
		TODO( "Temp fix " );
		s_RHIGlobals = {};

		m_UploadBuffer.Release();
		m_DescriptorHeapManager.Shutdown();

		if ( ULONG refCount = ForceDeleteIUnknown( m_Allocator.GetAddressOf() ) )
		{
			LOG( LogCategory::DirectX, Error, "D3D12MA allocator still has {0} references! - Destroying the allocator anyway", refCount );
		}

		if ( ULONG refCount = ForceDeleteIUnknown( m_DXGIAdapter.GetAddressOf() ) )
		{
			LOG( LogCategory::DirectX, Error, "DXGIAdapter still has {0} references! - Destroying the adapter anyway", refCount );
		}

		if ( ULONG refCount = ForceDeleteIUnknown( m_DXGIFactory.GetAddressOf() ) )
		{
			LOG( LogCategory::DirectX, Error, "DXGIFactory still has {0} references! - Destroying the factory anyway", refCount );
		}

		if ( ULONG refCount = ForceDeleteIUnknown( m_Device.GetAddressOf() ) )
		{
			LOG( LogCategory::DirectX, Error, "D3D12 device still has {0} references! - Destroying the device anyway", refCount );
		}

    #if RHI_DEBUG_ENABLED

		if ( ULONG refCount = ForceDeleteIUnknown( m_D3D12Debug.GetAddressOf() ) )
		{
			LOG( LogCategory::DirectX, Error, "D3D12 debug interface still has {0} references! - Destroying the debug interface anyway", refCount );
		}

		DumpDebug();
		m_DXGIDebug.Reset();
    #endif

		return true;
	}

    bool DynamicRHI_D3D12Impl::ExecuteCommandList( RHICommandListRef a_CommandList )
    {
		RHICommandList_D3D12Impl* cmdList = a_CommandList->As<RHICommandList_D3D12Impl>();
		if ( FAILED( cmdList->GraphicsCommandList()->Close() ) )
		{
			LOG( LogCategory::DirectX, Error, "Failed to close command list" );
			return false;
		}

		auto& cmdCtx = GetCommandContext( a_CommandList->Descriptor().QueueType );
		cmdCtx.CmdQueue->ExecuteCommandLists( 1, cmdList->CommandList.GetAddressOf() );
		a_CommandList->SetFenceValue( cmdCtx.Signal() );

		return true;
    }

    //////////////////////////////////////////////////////////////////////////
	// RESOURCE CREATION
	//////////////////////////////////////////////////////////////////////////

	RHIFenceRef DynamicRHI_D3D12Impl::CreateFence( const RHIFenceDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIFence_D3D12Impl>( a_Desc );
	}

	RHISamplerRef DynamicRHI_D3D12Impl::CreateSampler( const RHISamplerDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHISampler_D3D12Impl>( a_Desc );
	}

	RHITextureRef DynamicRHI_D3D12Impl::CreateTexture( const RHITextureDescriptor& a_Desc, Span<RHITextureSubresourceData> a_SubResourcesData )
	{
		return RHI::CreateNativeResource<RHITexture_D3D12Impl>( a_Desc, a_SubResourcesData );
	}

	RHIBufferRef DynamicRHI_D3D12Impl::CreateBuffer( const RHIBufferDescriptor& a_Desc, Span<const uint8_t> a_Data )
	{
		return RHI::CreateNativeResource<RHIBuffer_D3D12Impl>( a_Desc, a_Data );
	}

	RHIGraphicsPipelineStateRef DynamicRHI_D3D12Impl::CreateGraphicsPipelineState( const RHIGraphicsPipelineStateDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIGraphicsPipelineState_D3D12Impl>( a_Desc );
	}

	RHICommandListRef DynamicRHI_D3D12Impl::CreateCommandList( const RHICommandListDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHICommandList_D3D12Impl>( a_Desc );
	}

	RHIShaderModuleRef DynamicRHI_D3D12Impl::CreateShaderModule( const RHIShaderModuleDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIShaderModule_D3D12Impl>( a_Desc );
	}

	RHIBindingLayoutRef DynamicRHI_D3D12Impl::CreateBindingLayout( const RHIBindingLayoutDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIBindingLayout_D3D12Impl>( a_Desc );
	}

	RHIBindingSetRef DynamicRHI_D3D12Impl::CreateBindingSet( const RHIBindingSetDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHIBindingSet_D3D12Impl>( a_Desc );
	}

	RHISwapChainRef DynamicRHI_D3D12Impl::CreateSwapChain( const RHISwapChainDescriptor& a_Desc )
	{
		return RHI::CreateNativeResource<RHISwapChain_D3D12Impl>( a_Desc );
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
                DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS( DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL )
            );
			OutputDebugStringW( L"End of DirectX12 Debug Dump\n" );
        }
    }
#endif // RHI_DEBUG_ENABLED

} // namespace Tridium
