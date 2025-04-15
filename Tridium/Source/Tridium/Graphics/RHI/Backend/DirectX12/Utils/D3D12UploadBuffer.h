#pragma once
#include "../D3D12Common.h"
#include "../D3D12Context.h"
#include "../D3D12DescriptorHeap.h"
#include "ThirdParty/D3D12MemAlloc.h"

namespace Tridium::D3D12 {

	class UploadBuffer
	{
	public:
		bool Commit( size_t a_BufferSize, D3D12MA::Allocator& a_Allocator )
		{
			m_BufferSize = a_BufferSize;

			D3D12_RESOURCE_DESC bufferDesc = {};
			bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			bufferDesc.Width = m_BufferSize;
			bufferDesc.Height = 1;
			bufferDesc.DepthOrArraySize = 1;
			bufferDesc.MipLevels = 1;
			bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufferDesc.SampleDesc.Count = 1;
			bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			D3D12MA::ALLOCATION_DESC allocDesc = {};
			allocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

			return m_UploadBuffer.Commit( bufferDesc, allocDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, &a_Allocator );
		}

		void Release()
		{
			m_UploadBuffer.Release();
			m_BufferSize = 0;
			m_CurrentOffset = 0;
		}

		void* Allocate( size_t a_Size, size_t& o_Offset )
		{
			std::lock_guard lock( m_Mutex );

			// Align size to 256 bytes
			a_Size = ( a_Size + 255 ) & ~255;

			if ( m_CurrentOffset + a_Size > m_BufferSize )
			{
				// Wrap around ( circular allocation )
				Reset();
			}

			o_Offset = m_CurrentOffset;
			m_CurrentOffset += a_Size;

			void* mappedData = nullptr;
			D3D12_RANGE range = { 0, 0 };
			m_UploadBuffer.Resource->Map( 0, &range, &mappedData );

			return reinterpret_cast<uint8_t*>( mappedData ) + o_Offset;
		}

		void Reset()
		{
			m_CurrentOffset = 0;
		}

		ID3D12Resource* GetResource() const { return m_UploadBuffer.Resource.Get(); }

	private:
		ManagedResource m_UploadBuffer{};
		size_t m_BufferSize = 0;
		size_t m_CurrentOffset = 0;
		std::mutex m_Mutex{};
	};

} // namespace Tridium::D3D12