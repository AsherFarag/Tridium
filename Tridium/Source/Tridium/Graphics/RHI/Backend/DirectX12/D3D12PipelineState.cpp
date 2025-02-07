#include "tripch.h"
#include "D3D12PipelineState.h"

namespace Tridium {

    bool D3D12PipelineState::Commit( const void* a_Params )
    {
        return false;
    }

	bool D3D12PipelineState::Release()
	{
		return false;
	}

	bool D3D12PipelineState::IsValid() const
	{
		return false;
	}

	const void* D3D12PipelineState::NativePtr() const
	{
		return nullptr;
	}

} // namespace Tridium