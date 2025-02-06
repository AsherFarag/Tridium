#pragma once
#include <memory>

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
	//========================================================
	class OpaquePtr
	{
	public:
		using Deleter = void( * )( void* );

		OpaquePtr() : m_Ptr( nullptr, nullptr ) {}

		template<typename T>
		OpaquePtr( T* a_Ptr, Deleter a_Deleter = []( void* ptr ) { delete static_cast<T*>( ptr ); } )
			: m_Ptr( a_Ptr, a_Deleter ) {
		}

		// Nullptr assignment
		OpaquePtr( std::nullptr_t ) : m_Ptr( nullptr, nullptr ) {}

		// Move OpaquePtr
		OpaquePtr( OpaquePtr&& a_Other ) noexcept
			: m_Ptr( std::move( a_Other.m_Ptr ) ) {
		}

		// Move assignment OpaquePtr
		OpaquePtr& operator=( OpaquePtr&& a_Other ) noexcept
		{
			if ( this != &a_Other )
				m_Ptr = std::move( a_Other.m_Ptr );
			return *this;
		}

		// Move UniquePtr
		template<typename T>
		OpaquePtr( UniquePtr<T>&& a_Other ) noexcept
			: m_Ptr( std::move( a_Other ) ) {
		}

		// Move assignment UniquePtr
		template<typename T>
		OpaquePtr& operator=( UniquePtr<T>&& a_Other ) noexcept
		{
			m_Ptr.reset( a_Other.release() );
			m_Ptr.get_deleter() = []( void* ptr ) { delete static_cast<T*>( ptr ); };
			return *this;
		}

		// Delete copy constructor and assignment
		OpaquePtr( const OpaquePtr& ) = delete;
		OpaquePtr& operator=( const OpaquePtr& ) = delete;

		void* Get() const { return m_Ptr.get(); }

		template<typename T>
		T* Get() const { return static_cast<T*>( Get() ); }

		template<typename T = void>
		void Reset( T* a_Ptr = nullptr, Deleter a_Deleter = []( void* ptr ) { delete static_cast<T*>( ptr ); } )
		{
			m_Ptr.reset( a_Ptr );
			m_Ptr.get_deleter() = a_Deleter;
		}

		void Reset()
		{
			m_Ptr.reset();
		}

		void* Release()
		{
			return m_Ptr.release();
		}

		void Swap( OpaquePtr& a_Other )
		{
			m_Ptr.swap( a_Other.m_Ptr );
		}

		explicit operator bool() const { return m_Ptr.get() != nullptr; }
		bool operator==( std::nullptr_t ) const { return m_Ptr.get() == nullptr; }
		bool operator!=( std::nullptr_t ) const { return m_Ptr.get() != nullptr; }
		bool operator==( const OpaquePtr& a_Other ) const { return m_Ptr == a_Other.m_Ptr; }
		bool operator!=( const OpaquePtr& a_Other ) const { return m_Ptr != a_Other.m_Ptr; }
		bool operator==( const void* a_Ptr ) const { return m_Ptr.get() == a_Ptr; }
		bool operator!=( const void* a_Ptr ) const { return m_Ptr.get() != a_Ptr; }

	private:
		UniquePtr<void, Deleter> m_Ptr;
	};

}