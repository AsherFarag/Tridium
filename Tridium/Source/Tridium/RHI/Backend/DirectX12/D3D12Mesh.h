#pragma once
#include "D3D12Common.h"

namespace Tridium {

	class D3D12IndexBuffer final : public RHIIndexBuffer
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );

		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;
	};

	class D3D12VertexBuffer final : public RHIVertexBuffer
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );

		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool IsValid() const override;
		const void* NativePtr() const override;
	};

} // namespace Tridium