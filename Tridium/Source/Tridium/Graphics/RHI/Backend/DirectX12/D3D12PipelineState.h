#pragma once
#include "D3D12Common.h"
#include "D3D12RootSignature.h"

namespace Tridium {

	class D3D12PipelineState final : public RHIPipelineState
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

	private:
		ComPtr<ID3D12PipelineState> m_PipelineState;
		SharedPtr<D3D12RootSignature> m_RootSignature;
		D3D12_INPUT_ELEMENT_DESC m_VertexLayout[RHIQuery::MaxVertexAttributes];

	private:
		bool CommitGraphics();
		bool CommitCompute();
	};

} // namespace Tridium