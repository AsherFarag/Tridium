#pragma once
#include "D3D12.h"
#include "D3D12Util.h"
#include <Tridium/Graphics/RHI/DynamicRHI.h>
#include <Tridium/Graphics/RHI/RHIMesh.h>
#include <Tridium/Graphics/RHI/RHITexture.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>
#include <Tridium/Graphics/RHI/RHICommandList.h>
#include <Tridium/Graphics/RHI/RHIShader.h>
#include <Tridium/Graphics/RHI/RHIShaderBindingLayout.h>

DECLARE_LOG_CATEGORY( DirectX );

namespace Tridium {

	//=====================================================================
	// Type definitions for DirectX12 to avoid versioning issues
	namespace D3D12 {
		using Factory = IDXGIFactory7;
		using Device = ID3D12Device8;
		using CommandQueue = ID3D12CommandQueue;
		using CommandAllocator = ID3D12CommandAllocator;
		using GraphicsCommandList = ID3D12GraphicsCommandList;
		using Fence = ID3D12Fence1;
		using FenceValue = uint64_t;
		using SwapChain = IDXGISwapChain3;
		using Resource = ID3D12Resource2;
		using DescriptorHeap = ID3D12DescriptorHeap;
	#if RHI_DEBUG_ENABLED
		using D3D12Debug = ID3D12Debug;
		using DXGIDebug = IDXGIDebug1;
	#endif
	}
	//=====================================================================



	//=====================================================================
	// Concept definitions
	namespace D3D12::Concepts {
		template<typename T>
		concept IsIUnknown = std::is_base_of_v<IUnknown, T>;
	}
	//=====================================================================



	//=====================================================================
	// Com Pointer
	//  A templated smart pointer for COM objects.
	//  This pointer will automatically add and release references.
	//  Similar to Microsoft::WRL::ComPtr.
	template<typename T> requires D3D12::Concepts::IsIUnknown<T>
	class ComPtr
	{
	public:
		// Default empty constructor
		ComPtr() = default;

		// Construct by raw pointer (add ref)
		ComPtr( T* a_Ptr )
		{
			SetPointerAndAddRef( a_Ptr );
		}

		ComPtr( const ComPtr<T>& a_Other )
		{
			SetPointerAndAddRef( a_Other.m_Ptr );
		}

		ComPtr( ComPtr<T>&& a_Other ) noexcept
		{
			m_Ptr = a_Other.m_Ptr;
			a_Other.m_Ptr = nullptr;
		}

		~ComPtr()
		{
			ClearPointer();
		}

		ComPtr<T>& operator=( const ComPtr<T>& a_Other )
		{
			ClearPointer();
			SetPointerAndAddRef( a_Other.m_Ptr );
			return *this;
		}
		ComPtr<T>& operator=( ComPtr<T>&& a_Other )
		{
			ClearPointer();

			m_Ptr = a_Other.m_Ptr;
			a_Other.m_Ptr = nullptr;
			return *this;
		}
		ComPtr<T>& operator=( T* a_Other )
		{
			ClearPointer();
			SetPointerAndAddRef( a_Other );
			return *this;
		}

		ULONG Release()
		{
			return ClearPointer();
		}

		T* GetRef() const
		{
			if ( m_Ptr )
			{
				m_Ptr->AddRef();
				return m_Ptr;
			}
			return nullptr;
		}

		T* Get() const
		{
			return m_Ptr;
		}

		template<typename T>
		bool QueryInterface( ComPtr<T>& a_Other, HRESULT* a_ErrorCode = nullptr )
		{
			if ( m_Ptr )
			{
				HRESULT result = m_Ptr->QueryInterface( IID_PPV_ARGS( &a_Other ) );
				if ( a_ErrorCode ) *a_ErrorCode = result;
				return result == S_OK;
			}
			return false;
		}

		bool operator==( const ComPtr<T>& other ) const
		{
			return m_Ptr == other.m_Ptr;
		}
		bool operator==( const T* other ) const
		{
			return m_Ptr == other;
		}

		T* operator->()
		{
			return m_Ptr;
		}
		T** operator&()
		{
			return &m_Ptr;
		}

		T* const operator->() const
		{
			return m_Ptr;
		}

		T** const operator&() const
		{
			return &m_Ptr;
		}

		operator bool() const
		{
			return m_Ptr != nullptr;
		}

		operator T* ( )
		{
			return m_Ptr;
		}

	private:
		ULONG ClearPointer()
		{
			ULONG newRef = 0;
			if ( m_Ptr )
			{
				newRef = m_Ptr->Release();
				m_Ptr = nullptr;
			}

			return newRef;
		}

		void SetPointerAndAddRef( T* a_Ptr )
		{
			m_Ptr = a_Ptr;
			if ( m_Ptr )
			{
				m_Ptr->AddRef();
			}
		}

	private:
		T* m_Ptr = nullptr;
	};


}