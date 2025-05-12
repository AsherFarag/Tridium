#pragma once
#include "D3D12Common.h"

namespace Tridium {

	DECLARE_RHI_RESOURCE_IMPLEMENTATION( RHIGraphicsPipelineState_D3D12Impl, RHIGraphicsPipelineState )
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION_BODY( RHIGraphicsPipelineState_D3D12Impl, ERHInterfaceType::DirectX12 );

		RHIGraphicsPipelineState_D3D12Impl( const DescriptorType& a_Desc );
		bool Release() override;
		bool IsValid() const override { return PSO != nullptr; }
		const void* NativePtr() const override { return PSO.Get(); }

		ComPtr<ID3D12PipelineState> PSO;
		D3D12_INPUT_ELEMENT_DESC VertexLayout[RHIConstants::MaxVertexAttributes];
		uint32_t VertexLayoutSize = 0;
	};

} // namespace Tridium