#pragma once
#include "D3D12Common.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12GraphicsPipelineState, RHIGraphicsPipelineState )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12GraphicsPipelineState, ERHInterfaceType::DirectX12 );

		bool Commit( const RHIGraphicsPipelineStateDescriptor& a_Desc ) override;
		bool Release() override;
		bool IsValid() const override { return PSO != nullptr; }
		const void* NativePtr() const override { return PSO.Get(); }

		ComPtr<ID3D12PipelineState> PSO;
		D3D12_INPUT_ELEMENT_DESC VertexLayout[RHIConstants::MaxVertexAttributes];
	};

} // namespace Tridium