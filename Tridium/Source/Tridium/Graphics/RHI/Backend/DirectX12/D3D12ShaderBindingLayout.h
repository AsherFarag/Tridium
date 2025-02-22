#pragma once
#include "D3D12Common.h"

namespace Tridium {

	class D3D12ShaderBindingLayout : public RHIShaderBindingLayout
	{
	public:
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

		ComPtr<ID3D12RootSignature> m_RootSignature;
	};

} // namespace Tridium