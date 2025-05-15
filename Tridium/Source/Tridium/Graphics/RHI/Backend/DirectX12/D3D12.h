#pragma once
#include <Tridium/Graphics/RHI/RHIConfig.h>

#ifndef NOMINMAX
	#define NOMINMAX
#endif

#include <d3d12.h>
#include "ThirdParty/d3dx12.h"
#include <dxgi1_6.h>
	
#if RHI_DEBUG_ENABLED
	#include <d3d12sdklayers.h>
	#include <dxgidebug.h>
#endif

#include <type_traits>

namespace Tridium::D3D12 {

	[[nodiscard]] inline auto CalcSubresource( uint32_t a_MipSlice, uint32_t a_ArraySlice, uint32_t a_PlaneSlice, uint32_t a_MipLevels, uint32_t a_ArraySize ) noexcept
	{
		return ((a_MipSlice)+(a_ArraySlice) * (a_MipLevels)+(a_PlaneSlice) * (a_MipLevels) * (a_ArraySize));
	}

	//  Converts a number of bytes to a number of WORDs. A WORD is 2 bytes.
	[[nodiscard]] inline uint32_t NumWORDsFromBytes( uint32_t a_Value ) noexcept
	{
		return (a_Value + sizeof( uint16_t ) - 1) / sizeof( uint16_t );
	}

	// Converts a number of WORDs to a number of bytes. A WORD is 2 bytes.
	[[nodiscard]] inline uint32_t NumBytesFromWORDs( uint32_t a_Value ) noexcept
	{
		return  a_Value * sizeof( uint16_t );
	}

	// Converts a number of bytes to a number of DWORDs. A DWORD is 4 bytes.
	[[nodiscard]] inline uint32_t NumDWORDsFromBytes( uint32_t a_Value ) noexcept
	{
		return (a_Value + sizeof( uint32_t ) - 1) / sizeof( uint32_t );
	}

	// Converts a number of DWORDs to a number of bytes. A DWORD is 4 bytes.
	[[nodiscard]] inline uint32_t NumBytesFromDWORDs( uint32_t a_Value ) noexcept
	{
		return  a_Value * sizeof( uint32_t );
	}

	// Converts a number of bytes to a number of QWORDs. A QWORD is 8 bytes.
	[[nodiscard]] inline uint32_t NumQWORDsFromBytes( uint32_t a_Value ) noexcept
	{
		return (a_Value + sizeof( uint64_t ) - 1) / sizeof( uint64_t );
	}

	// Converts a number of QWORDs to a number of bytes. A QWORD is 8 bytes.
	[[nodiscard]] inline uint32_t NumBytesFromQWORDs( uint32_t a_Value ) noexcept
	{
		return  a_Value * sizeof( uint64_t );
	}

	//=====================================================================
	// Com Pointer
	//  A templated smart pointer for COM objects.
	//  This pointer will automatically add and release references.
	//  Similar to Microsoft::WRL::ComPtr.
	template<typename T> requires std::is_base_of_v<class IUnknown, T>
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
		bool QueryInterface( ComPtr<T>& a_Other, HRESULT* a_ErrorCode = nullptr ) const
		{
			if ( m_Ptr )
			{
				HRESULT result = m_Ptr->QueryInterface( IID_PPV_ARGS( &a_Other ) );
				if ( a_ErrorCode ) *a_ErrorCode = result;
				return result == S_OK;
			}
			return false;
		}

		bool operator==( const ComPtr<T>& a_Other ) const
		{
			return m_Ptr == a_Other.m_Ptr;
		}
		bool operator==( const T* a_Other ) const
		{
			return m_Ptr == a_Other;
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