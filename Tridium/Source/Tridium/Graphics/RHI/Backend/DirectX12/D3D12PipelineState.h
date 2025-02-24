#pragma once
#include "D3D12Common.h"

namespace Tridium {

	class D3D12PipelineState final : public RHIPipelineState
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		ComPtr<ID3D12PipelineState> PSO;
		D3D12_INPUT_ELEMENT_DESC VertexLayout[RHIQuery::MaxVertexAttributes];

	private:
		bool CommitGraphics();
		bool CommitCompute();
	};

} // namespace Tridium