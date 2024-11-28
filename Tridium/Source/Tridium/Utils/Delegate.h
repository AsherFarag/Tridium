#pragma once
#include <Tridium/Core/GUID.h>

namespace Tridium {
	using DelegateHandle = UID<uint32_t>;

	//////////////////////////////////////////////////////////////////////////
	// Delegate
	//////////////////////////////////////////////////////////////////////////
	template <typename _Return, typename... _Args>
	class Delegate
	{
		template <typename T>
		using MemberFunction = _Return( T::* )( _Args... );
		using Invoker = _Return( * )( void* a_Object, void* a_Function, _Args... );
	public:
		using Function = _Return( * )( _Args... );

		Delegate() : m_Function( nullptr ), m_Object( nullptr ), m_Invoker( nullptr ) {}

		Delegate( Function a_Function )
		{
			Bind( a_Function );
		}

		template <typename T>
		Delegate( T* a_Object, MemberFunction<T> a_Function )
		{
			Bind( a_Object, a_Function );
		}

		inline _Return operator()( _Args... a_Args ) const { return Invoke( a_Args... ); }

		_Return Invoke( _Args... a_Args ) const 
		{ 
			return m_Invoker( m_Object, m_Function, std::forward<_Args>( a_Args )... );
		}

		void Bind( Function a_Function )
		{
			m_Function = a_Function;
			m_Object = nullptr;
			m_Invoker = +[]( void* a_Object, void* a_Function, _Args... a_Args ) -> _Return
				{
					Function function = static_cast<Function>( a_Function );
					return function( std::forward<_Args>( a_Args )... );
				};
		}

		template <typename T>
		void Bind( T* a_Object, MemberFunction<T> a_Function )
		{
			m_Function = *reinterpret_cast<Function*>( &a_Function );
			m_Object = a_Object;
			m_Invoker = +[]( void* a_Object, void* a_Function, _Args... a_Args ) -> _Return
				{
					T* object = static_cast<T*>( a_Object );
					MemberFunction<T> function = *reinterpret_cast<MemberFunction<T>*>( &a_Function );
					return ( object->*function )( std::forward<_Args>( a_Args )... );
				};
		}

		void Clear()
		{
			m_Function = nullptr;
			m_Object = nullptr;
			m_Invoker = nullptr;
		}

	protected:
		Invoker m_Invoker;
		Function m_Function;
		void* m_Object;
	};

	//////////////////////////////////////////////////////////////////////////
	// Multicast Delegate
	//////////////////////////////////////////////////////////////////////////
	template <typename... _Args>
	class MulticastDelegate
	{
	public:
		using DelegateType = Delegate<void, _Args...>;
		using DelegateList = std::unordered_map<DelegateHandle, DelegateType>;

		MulticastDelegate() = default;

		// Calls all delegates in the list with the given arguments
		void Broadcast( _Args... a_Args ) const
		{
			for ( const auto& [handle, delegate] : m_Delegates )
			{
				delegate.Invoke( std::forward<_Args>( a_Args )... );
			}
		}

		// Adds a new delegate to the list
		DelegateHandle Add( typename DelegateType::Function a_Function )
		{
			return Add( DelegateType( a_Function ) );
		}

		// Adds a new delegate to the list
		template<typename T>
		DelegateHandle Add( T* a_Object, typename DelegateType::template MemberFunction<T> a_Function )
		{
			return Add( DelegateType( a_Object, a_Function ) );
		}

		// Adds a new delegate to the list
		DelegateHandle Add( const DelegateType& a_Delegate )
		{
			DelegateHandle handle = DelegateHandle::Create();
			m_Delegates[handle] = a_Delegate;
			return handle;
		}

		// Removes a delegate from the list
		bool Remove( const DelegateHandle& a_Handle )
		{
			if ( auto it = m_Delegates.find( a_Handle ); it != m_Delegates.end() )
			{
				m_Delegates.erase( it );
				return true;
			}
			return false;
		}

		// Removes all delegates from the list
		void Clear()
		{
			m_Delegates.clear();
		}

		// Returns the number of delegates in the list
		size_t Size() const
		{
			return m_Delegates.size();
		}

		// Returns true if there are no delegates in the list
		bool IsEmpty() const
		{
			return m_Delegates.empty();
		}

	protected:
		DelegateList m_Delegates;
	};

} // namespace Tridium