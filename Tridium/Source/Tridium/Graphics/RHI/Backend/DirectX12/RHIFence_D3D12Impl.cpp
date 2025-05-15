#include "tripch.h"
#include "RHI_D3D12Impl.h"

namespace Tridium::D3D12 {

	RHIFence_D3D12Impl::RHIFence_D3D12Impl( const DescriptorType& a_Desc )
		: RHIFence( a_Desc )
    {
		const auto& device = GetD3D12RHI()->GetD3D12Device();
		const auto flags = D3D12_FENCE_FLAG_NONE;

        if ( !ASSERT( SUCCEEDED( device->CreateFence( 0, flags, IID_PPV_ARGS( &m_Fence ) ) ),
			"Failed to create fence!" ) )
        {
			return;
        }

		D3D12_SET_DEBUG_NAME( m_Fence.Get(), m_Desc.Name );

		m_FenceEvent = CreateEvent( 
			nullptr,    // default security attributes
			TRUE,	    // manual-reset event
			FALSE,	    // initial state is nonsignaled
			nullptr     // object name
        );
    }

	RHIFence_D3D12Impl::~RHIFence_D3D12Impl()
	{
		Release();
	}

    uint64_t RHIFence_D3D12Impl::GetCompletedValue()
    {
		uint64_t value = m_Fence->GetCompletedValue();
		ENSURE( value != UINT64_MAX, "If the device is removed, the fence value will be UINT64_MAX" );
		return uint64_t( value );
    }

	void RHIFence_D3D12Impl::Signal( uint64_t a_Value )
	{
		ASSERT( Descriptor().Type == ERHIFenceType::General, "Signaling on a CPUWaitOnly fence, fence type must be ERHIFenceType::General" );
		m_Fence->Signal( a_Value );
	}

	void RHIFence_D3D12Impl::Wait( uint64_t a_Value )
	{
		if ( GetCompletedValue() >= a_Value )
		{
			return;
		}

		if ( m_FenceEvent )
		{
			m_Fence->SetEventOnCompletion( a_Value, m_FenceEvent );
			WaitForSingleObject( m_FenceEvent, INFINITE );
		}
		else
		{
			while ( GetCompletedValue() < a_Value )
			{
				TODO( "This is a busy wait, we should probably use an event?" );
				std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
			}
		}
	}

	bool RHIFence_D3D12Impl::Release()
	{
		m_Fence.Release();
		if ( m_FenceEvent != nullptr )
		{
			CloseHandle( m_FenceEvent );
			m_FenceEvent = nullptr;
		}
		return true;
	}

	bool RHIFence_D3D12Impl::IsValid() const
	{
		return m_Fence != nullptr;
	}

	const void* RHIFence_D3D12Impl::NativePtr() const
	{
		return m_Fence.Get();
	}

}
