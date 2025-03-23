#pragma once
#include "D3D12Common.h"

namespace Tridium {

	class D3D12GraphicsPipelineState final : public RHIGraphicsPipelineState
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override { return PSO != nullptr; }
		const void* NativePtr() const override { return PSO.Get(); }

		ComPtr<ID3D12PipelineState> PSO;
		D3D12_INPUT_ELEMENT_DESC VertexLayout[RHIConstants::MaxVertexAttributes];
	};

} // namespace Tridium