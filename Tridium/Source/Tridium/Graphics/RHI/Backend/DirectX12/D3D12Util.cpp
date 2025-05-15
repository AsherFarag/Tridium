#include "tripch.h"
#include "RHI_D3D12Impl.h"

namespace Tridium::D3D12 {

	bool ManagedResource::Commit( const D3D12_RESOURCE_DESC& a_ResourceDesc, const D3D12MA::ALLOCATION_DESC& a_AllocDesc, D3D12_RESOURCE_STATES a_InitialState, const D3D12_CLEAR_VALUE* a_ClearValue, D3D12MA::Allocator* a_Allocator )
	{
		if ( a_Allocator == nullptr )
		{
			a_Allocator = GetD3D12RHI()->GetAllocator().Get();
		}

		HRESULT hr = a_Allocator->CreateResource(
			&a_AllocDesc, &a_ResourceDesc,
			a_InitialState, a_ClearValue,
			&Allocation, IID_PPV_ARGS( &Resource ) );

		return SUCCEEDED( hr );
	}

} // namespace Tridium::D3D12