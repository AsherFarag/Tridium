#pragma once
#include "D3D12Common.h"

namespace Tridium {

	//=================================================
	// D3D12 Shader Module
	//  Known as a Shader Blob in D3D12.
	//=================================================
	class D3D12ShaderModule final : public RHIShaderModule
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );
		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;

	private:
		ComPtr<ID3DBlob> m_ShaderBlob;
	};

} // namespace Tridium