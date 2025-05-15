#pragma once
#include <memory>
#include <Tridium/Core/Assert.h>

namespace Tridium {

	// A scoped pointer that frees the memory when this object goes out of scope.
	template <typename T>
	class Scope
	{
	public:
		Scope( T* a_Ptr = nullptr )
			: m_Ptr( a_Ptr ), m_Retired( false ) {}

		~Scope()
		{
			if ( m_Ptr && !m_Retired )
				delete m_Ptr;
		}

		T* operator->() const { return m_Ptr; }
		T& operator*() const { return *m_Ptr; }

		T* Get() const { return m_Ptr; }
		void Set( T* a_Ptr ) { m_Ptr = a_Ptr; m_Retired = false; }
		bool IsRetired() const { return m_Retired; }
		void Retire() { m_Retired = true; }

		bool operator==( std::nullptr_t ) const { return m_Ptr == nullptr; }
		bool operator!=( std::nullptr_t ) const { return m_Ptr != nullptr; }
		explicit operator bool() const { return m_Ptr != nullptr; }

	private:
		T* m_Ptr;
		bool m_Retired;
	};

	template <typename T, typename _Deleter = std::default_delete<T>>
	using UniquePtr = std::unique_ptr<T, _Deleter>;

	template<typename T, typename ... Args>
	constexpr UniquePtr<T> MakeUnique( Args&& ... args )
	{
		return std::make_unique<T>( std::forward<Args>( args )... );
	}

	template<typename T>
	using SharedPtr = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr SharedPtr<T> MakeShared( Args&& ... args )
	{
		return std::make_shared<T>( std::forward<Args>( args )... );
	}

	template<typename _To, typename _From>
	inline constexpr SharedPtr<_To> SharedPtrCast( const SharedPtr<_From>& a_Other )
	{
		return std::static_pointer_cast<_To>( a_Other );
	}

	template<typename T>
	using EnableSharedFromThis = std::enable_shared_from_this<T>;

	// Type alias for std::weak_ptr
	template<typename T>
	using WeakPtr = std::weak_ptr<T>;

	// Function to convert std::shared_ptr to std::weak_ptr
	template<typename T>
	constexpr WeakPtr<T> MakeWeakPtr( const SharedPtr<T>& a_SharedPtr )
	{
		return WeakPtr<T>( a_SharedPtr );
	}


	//========================================================
	// Opaque Pointer
	//	Functions as a UniquePtr<void> with a custom deleter.
	//  Note: The deleter is a static function pointer.
	//========================================================
	class OpaquePtr
	{
	public:
		using Deleter = void( * )( void* );

		template<typename T>
		static constexpr Deleter DefaultDeleter() { return +[]( void* a_Ptr ) { delete Cast<T*>( a_Ptr ); }; }

		OpaquePtr() = default;
		OpaquePtr( std::nullptr_t ) {}
		OpaquePtr( const OpaquePtr& ) = delete;
		OpaquePtr& operator=( const OpaquePtr& ) = delete;

		OpaquePtr( OpaquePtr&& a_Other ) noexcept
			: m_Ptr( std::exchange( a_Other.m_Ptr, nullptr ) )
			, m_Deleter( std::exchange( a_Other.m_Deleter, nullptr ) )
		{}

		template<typename T>
		OpaquePtr( T* a_Ptr, Deleter a_Deleter = DefaultDeleter<T>() )
			: m_Ptr( a_Ptr ), m_Deleter( a_Deleter ) {}

		OpaquePtr& operator=( OpaquePtr&& a_Other ) noexcept
		{
			m_Ptr = std::exchange( a_Other.m_Ptr, nullptr );
			m_Deleter = std::exchange( a_Other.m_Deleter, nullptr );
			return *this;
		}

		template<typename T>
		OpaquePtr( UniquePtr<T>&& a_Other ) noexcept
			: m_Ptr( a_Other.release() )
			, m_Deleter( DefaultDeleter<T>() )
		{
		}

		template<typename T>
		OpaquePtr& operator=( UniquePtr<T>&& a_Other ) noexcept
		{
			m_Ptr = a_Other.release();
			m_Deleter = DefaultDeleter<T>();
			return *this;
		}

		~OpaquePtr() { Reset(); }

		void* Get() const { return m_Ptr; }

		void Reset()
		{
			if ( m_Ptr )
			{
				ASSERT( m_Deleter != nullptr );
				m_Deleter( m_Ptr );
			}

			m_Ptr = nullptr;
			m_Deleter = nullptr;
		}

		void* Release()
		{
			m_Deleter = nullptr;
			return std::exchange( m_Ptr, nullptr );
		}

		void Swap( OpaquePtr& a_Other )
		{
			std::swap( m_Ptr, a_Other.m_Ptr );
			std::swap( m_Deleter, a_Other.m_Deleter );
		}

		template<typename T>
		T* Get() const { return Cast<T*>( Get() ); }

		template<typename T>
		void Reset( T* a_Ptr, Deleter a_Deleter = DefaultDeleter<T>() )
		{
			Reset();
			m_Ptr = a_Ptr;
			m_Deleter = a_Deleter;
		}

		explicit operator bool() const { return m_Ptr != nullptr; }
		bool operator==( std::nullptr_t ) const { return m_Ptr == nullptr; }
		bool operator!=( std::nullptr_t ) const { return m_Ptr != nullptr; }
		bool operator==( const OpaquePtr& a_Other ) const { return m_Ptr == a_Other.m_Ptr; }
		bool operator!=( const OpaquePtr& a_Other ) const { return m_Ptr != a_Other.m_Ptr; }
		bool operator==( const void* a_Ptr ) const { return m_Ptr == a_Ptr; }
		bool operator!=( const void* a_Ptr ) const { return m_Ptr != a_Ptr; }

	private:
		void* m_Ptr{ nullptr };
		Deleter m_Deleter{ nullptr };
	};

}