#pragma once
#include <Tridium/Core/GUID.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Utils/TypeTraits.h>
#include <unordered_map>

namespace Tridium {

	// NOTE:
	// These delegates are not thread-safe. They are intended to be used in a single-threaded environment.
	// They are also not memory-safe, as they do not perform any reference counting on the objects they hold.
	// You must remove the Delegate from the MulticastDelegate before the object it points to is destroyed.

	using DelegateHandle = std::pair<UID<uint16_t>, SharedPtr<void>>;

	//////////////////////////////////////////////////////////////////////////
	// Delegate
	//////////////////////////////////////////////////////////////////////////

	template <typename _Return = void, typename... _Args>
	class Delegate
	{
		using MemberFunction = _Return( * )( void*, _Args... );
		using Deleter = void( * )( void* a_Object );

		template <typename T>
		void DeleterFunction( void* a_Object )
		{
			delete static_cast<T*>( a_Object );
		}

		template <auto _Func>
		static _Return Invocation( void* a_Instance, _Args... a_Args )
		{
			using ObjectType = std::get_member_function_type_t<decltype( _Func )>;
			ObjectType* Object = (ObjectType*)a_Instance;
			return ( Object->*_Func )( std::forward< _Args >( a_Args )... );
		}

		template <typename T>
		static _Return FunctorInvocation( void* a_Instance, _Args... a_Args )
		{
			T* Object = static_cast<T*>( a_Instance );
			return ( *Object )( std::forward<_Args>( a_Args )... );
		}

	public:
		using Function = _Return( * )( _Args... );

		Delegate() : m_Function( nullptr ), m_Object( nullptr ), m_Deleter(nullptr) {}

		template <Function _StaticFunc>
		Delegate()
		{
			Bind<_StaticFunc>();
		}

		Delegate( Function a_StaticFunction )
		{
			m_Function = (void*)a_StaticFunction;
			m_Object = nullptr;
			m_Deleter = nullptr;
		}

		template <auto _Func, typename T>
		Delegate( T* a_Object )
		{
			Bind<_Func>( a_Object );
		}

		template<typename T>
		Delegate( T&& a_Functor )
		{
			Bind( std::forward<T>( a_Functor ) );
		}

		Delegate( const Delegate& ) = delete;
		Delegate& operator=( const Delegate& ) = delete;

		~Delegate()
		{
			if ( IsObjectOwned() )
			{
				m_Deleter( m_Object );
			}
		}

		_Return operator()( _Args... a_Args ) const { return Invoke( a_Args... ); }

		_Return Invoke( _Args... a_Args ) const
		{
			if ( m_Object )
			{
				return ( (MemberFunction)m_Function )( m_Object, std::forward<_Args>( a_Args )... );
			}
			else if ( m_Function )
			{
				return ( (Function)m_Function )( std::forward<_Args>( a_Args )... );
			}

			TE_CORE_ASSERT( false, "Delegate is not bound to a function!" );
			return _Return();
		}

		// Binds a member function and a non-owning pointer to an instance, to the delegate
		template <auto _MemFunc, typename T>
		void Bind( T* a_Object )
		{
			static_assert( !std::is_pointer_v<T>, "Cannot bind a pointer to a pointer." );
			static_assert( std::is_member_function_pointer_v<decltype( _MemFunc )>, "Cannot bind a non-member function to a delegate using this method." );

			// Clean up any previous object
			Clear();

			m_Function = (void*)Invocation<_MemFunc>;
			m_Object = a_Object;
			m_Deleter = nullptr;
		}

		// Binds a member function and an owning instance to the delegate
		template <auto _Func, typename T>
		void Bind( T&& a_Object )
		{
			static_assert( std::is_member_function_pointer_v<decltype( _Func )>, "Cannot bind a non-member function to a delegate using this method." );
			static_assert( !std::is_pointer_v<T>, "Cannot bind a pointer to an owning instance." );

			// Clean up any previous object
			Clear();

			m_Function = (void*)Invocation<_Func>;
			m_Object = new T( std::forward<T>( a_Object ) );
			m_Deleter = (Deleter*)DeleterFunction<T>;
		}

		// Binds a static function to the delegate
		template <Function _StaticFunc>
		void Bind()
		{
			// Clean up any previous object
			Clear();

			m_Function = (void*)_StaticFunc;
			m_Object = nullptr;
			m_Deleter = nullptr;
		}

		// Binds a functor to the delegate
		// A functor can be a lambda, a std::function, or any other callable object that overloads the operator()
		// The Functor must be invokable with the same arguments as the delegate
		template <typename T>
		void Bind( T&& a_Functor )
		{
			using FunctorType = std::remove_reference_t<T>;
			static_assert( std::is_invocable_v<FunctorType, _Args...>, "Cannot bind a non-invokable object to a delegate." );

			// Clean up any previous object
			Clear();

			// Case 1: If the functor is a pointer to an invokable object
			if constexpr ( std::is_pointer_v<FunctorType> )
			{
				m_Function = (void*)FunctorInvocation<std::remove_pointer_t<FunctorType>>;
				m_Object = a_Functor;
				m_Deleter = nullptr; 
			}
			// Case 2: If the functor is a reference to an invokable object
			else if constexpr ( std::is_lvalue_reference_v<decltype( a_Functor )> )
			{
				m_Function = (void*)FunctorInvocation<std::remove_pointer_t<FunctorType>>;
				m_Object = &a_Functor;
				m_Deleter = nullptr;
			}
			// Case 3: If the functor is an owning invokable object
			else if constexpr ( std::is_rvalue_reference_v<decltype( a_Functor )> )
			{
				m_Function = (void*)FunctorInvocation<FunctorType>;
				m_Object = new FunctorType( std::forward<T>( a_Functor ) );
				m_Deleter = +[]( void* a_Object ) { delete static_cast<FunctorType*>( a_Object ); };
			}
			else
			{
				//static_assert( false, "Invalid functor type." );
			}
		}

		bool IsBound() const
		{
			return m_Function != nullptr;
		}

		bool IsStatic() const
		{
			return IsBound() && m_Object == nullptr;
		}

		void Clear()
		{
			if ( IsObjectOwned() && m_Object )
			{
				m_Deleter( m_Object );
			}

			m_Function = nullptr;
			m_Deleter = nullptr;
			m_Object = nullptr;
		}

		bool IsObjectOwned() const
		{
			return m_Deleter != nullptr;
		}

	protected:
		void* m_Function;
		void* m_Object;
		Deleter m_Deleter;
	};



	//////////////////////////////////////////////////////////////////////////
	// Multicast Delegate
	//////////////////////////////////////////////////////////////////////////

	template <typename _Return = void, typename... _Args>
	class MulticastDelegate
	{
	public:
		using DelegateType = Delegate<_Return, _Args...>;
		using DelegateList = std::unordered_map<DelegateHandle::first_type, WeakPtr<DelegateType>>;

		MulticastDelegate() = default;

		// Calls all delegates in the list with the given arguments
		void Broadcast( _Args... a_Args )
		{
			m_IsBroadcasting = true;
			for ( const auto& [handle, delegate] : this->m_Delegates )
			{
				if ( SharedPtr<DelegateType> lockedDel = delegate.lock() )
					lockedDel->Invoke( std::forward<_Args>( a_Args )... );
			}
			m_IsBroadcasting = false;
		}

		template <auto _Func, typename T>
		DelegateHandle Add( T* a_Object )
		{
			SharedPtr<DelegateType> delegate = MakeShared<DelegateType>();
			delegate->Bind<_Func>( a_Object );
			DelegateHandle handle = MakeHandle( delegate );
			return handle;
		}

		// Adds a new delegate to the list
		DelegateHandle Add( DelegateType&& a_Delegate )
		{
			return MakeHandle( MakeShared<DelegateType>( std::move( a_Delegate ) ) );
		}

		// Removes a delegate from the list
		bool Remove( const DelegateHandle& a_Handle )
		{
			if ( auto it = m_Delegates.find( a_Handle.first ); it != m_Delegates.end() )
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

		// Returns true if the delegate is currently broadcasting
		bool IsBroadcasting() const
		{
			return m_IsBroadcasting;
		}

	protected:
		DelegateHandle MakeHandle( const SharedPtr<DelegateType>& a_Delegate )
		{
			DelegateHandle handle = std::make_pair( DelegateHandle::first_type::Create(), SharedPtr<void>( a_Delegate ) );
			m_Delegates[handle.first] = a_Delegate;
			return handle;
		}

	protected:
		DelegateList m_Delegates;
		bool m_IsBroadcasting = false;
	};

} // namespace Tridium