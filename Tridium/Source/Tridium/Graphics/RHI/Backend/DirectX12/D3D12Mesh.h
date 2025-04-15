#pragma once
#include "D3D12Common.h"

namespace Tridium {

	//DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12IndexBuffer, RHIIndexBuffer )
	//{
	//public:
	//	RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12IndexBuffer, ERHInterfaceType::DirectX12 );

	//	virtual bool Commit( const void* a_Params ) override;
	//	virtual bool Release() override;
	//	virtual bool IsValid() const override;
	//	virtual size_t GetSizeInBytes() const override;
	//	virtual const void* NativePtr() const override { return IBO.Resource.Get(); }
	//	virtual bool Read( Array<Byte>& o_Data, size_t a_SrcOffset = 0 ) override;
	//	virtual bool IsReadable() const override;
	//	virtual bool Write( const Span<const Byte>& a_Data, size_t a_DstOffset = 0 );
	//	virtual bool IsWritable() const override;
	//	virtual bool IsReady() const override;
	//	virtual void Wait() override;

	//	D3D12::ManagedResource IBO{};

	//private:
	//	uint64_t m_CopyFenceValue = 0;
	//};

	//DECLARE_RHI_RESOURCE_IMPLEMENTATION( D3D12VertexBuffer, RHIVertexBuffer )
	//{
	//public:
	//	RHI_RESOURCE_IMPLEMENTATION_BODY( D3D12VertexBuffer, ERHInterfaceType::DirectX12 );

	//	virtual bool Commit( const void* a_Params ) override;
	//	virtual bool Release() override;
	//	virtual bool IsValid() const override { return VBO.Valid(); }
	//	virtual size_t GetSizeInBytes() const override;
	//	virtual const void* NativePtr() const override { return VBO.Resource.Get(); }
	//	virtual bool Read( Array<Byte>& o_Data, size_t a_SrcOffset = 0 ) override;
	//	virtual bool IsReadable() const override;
	//	virtual bool Write( const Span<const Byte>& a_Data, size_t a_DstOffset = 0 ) override;
	//	virtual bool IsWritable() const override;
	//	virtual bool IsReady() const override;
	//	virtual void Wait() override;

	//	D3D12::ManagedResource VBO{};

	//private:
	//	uint64_t m_CopyFenceValue = 0;
	//};

} // namespace Tridium