#include "tripch.h"
#include "D3D12Fence.h"
#include "D3D12DynamicRHI.h"

namespace Tridium {

    D3D12Fence::~D3D12Fence()
    {
		Release();
    }

	bool D3D12Fence::Commit( const RHIFenceDescriptor& a_Desc )
    {
		m_Desc = a_Desc;
		const auto& device = GetD3D12RHI()->GetDevice();
		const auto flags = D3D12_FENCE_FLAG_NONE;
        if ( FAILED( device->CreateFence( 0, flags, IID_PPV_ARGS( &m_Fence ) ) ) )
        {
			LOG( LogCategory::DirectX, Error, "Failed to create fence" );
			return false;
        }

		D3D12_SET_DEBUG_NAME( m_Fence.Get(), m_Desc.Name );

		m_FenceEvent = CreateEvent( 
			nullptr,    // default security attributes
			TRUE,	    // manual-reset event
			FALSE,	    // initial state is nonsignaled
			nullptr     // object name
        );

		return true;
    }

    uint64_t D3D12Fence::GetCompletedValue()
    {
		uint64_t value = m_Fence->GetCompletedValue();
		ENSURE_LOG( value != UINT64_MAX, "If the device is removed, the fence value will be UINT64_MAX" );
		return uint64_t( value );
    }

	void D3D12Fence::Signal( uint64_t a_Value )
	{
		ASSERT_LOG( Descriptor().Type == ERHIFenceType::General, "Signaling on a CPUWaitOnly fence, fence type must be ERHIFenceType::General" );
		m_Fence->Signal( a_Value );
	}

	void D3D12Fence::Wait( uint64_t a_Value )
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

	bool D3D12Fence::Release()
	{
		m_Fence.Release();
		if ( m_FenceEvent != nullptr )
		{
			CloseHandle( m_FenceEvent );
			m_FenceEvent = nullptr;
		}
		return true;
	}

	bool D3D12Fence::IsValid() const
	{
		return m_Fence != nullptr;
	}

	const void* D3D12Fence::NativePtr() const
	{
		return m_Fence.Get();
	}

}
