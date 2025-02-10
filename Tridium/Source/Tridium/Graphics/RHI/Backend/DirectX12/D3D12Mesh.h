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

	private:
		ComPtr<D3D12::Resource> m_Buffer;
	};

	class D3D12VertexBuffer final : public RHIVertexBuffer
	{
	public:
		RHI_RESOURCE_IMPLEMENTATION( DirectX12 );

		bool Commit( const void* a_Params ) override;
		bool Release() override;
		bool Write( const Span<const Byte>& a_Data, size_t a_DstOffset = 0 ) override;
		bool IsWritable() const override;
		bool IsValid() const override;
		const void* NativePtr() const override;

	private:
		ComPtr<D3D12::Resource> m_Resource;
		ComPtr<D3D12::DescriptorHeap> m_DescriptorHeap;
	};

} // namespace Tridium