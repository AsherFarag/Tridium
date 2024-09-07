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

	template <typename T>
	using UniquePtr = std::unique_ptr<T>;

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
	inline constexpr SharedPtr<_To> SharedPtrCast( const SharedPtr<_From>& other )
	{
		return std::static_pointer_cast<_To>( other );
	}

	// Type alias for std::weak_ptr
	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	// Function to convert std::shared_ptr to std::weak_ptr
	template<typename T>
	constexpr WeakRef<T> MakeWeakRef( const std::shared_ptr<T>& sharedPtr )
	{
		return WeakRef<T>( sharedPtr );
	}

}