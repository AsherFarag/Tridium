#pragma once
#include <type_traits>
#include <Tridium/Core/Assert.h>
#include <Tridium/Containers/Array.h>
#include <Tridium/Containers/Map.h>

TODO( "Move this somewhere else and probably dont expand std" );
namespace std {
	template<typename T, typename U>
	struct helper : helper<T, decltype(&U::operator())> {};

	template<typename T, typename C, typename R, typename... A>
	struct helper<T, R( C::* )(A...) const>
	{
		static constexpr bool value = std::is_convertible<T, R( * )(A...)>::value;
	};

	template<typename T>
	struct is_stateless
	{
		static constexpr bool value = helper<T, T>::value;
	};

	template<typename T>
	static constexpr bool is_stateless_v = is_stateless<T>::value;
}

namespace Tridium {

	template<typename>
	class Invoker;

	//==============================================================
	// Invoker
	//	An object that can call a method with minimal overhead.
	//	The method can be a free function, a member function, or a lambda.
	//	Users are responsible for unbinding instances before destroying them.
	template<typename _Return, typename... _Args>
	class Invoker<_Return( _Args... )>
	{
	public:
		using ReturnType = _Return;
		using FunctionType = ReturnType( const void*, _Args... );

		// Default Constructor
		constexpr Invoker() = default;

		// Free/Member Function Constructor
		constexpr Invoker( FunctionType* a_Function, const void* a_Instance = nullptr )
		{
			Bind( a_Function, a_Instance );
		}

		// Lambda Constructor
		template<typename _Functor> requires 
			std::is_invocable_r_v<ReturnType, _Functor, _Args...> // _Functor must be invocable with the same arguments and return type as the function
			&& !std::is_member_pointer_v<_Functor>                // _Functor must not be a member function pointer
			&& std::is_stateless_v<_Functor>                      // _Functor must be a static lambda (no captures)
		constexpr Invoker( _Functor a_Functor )
		{
			Bind<a_Functor>();
		}

		// Copy Constructor
		constexpr Invoker( const Invoker& ) = default;

		// Move Constructor
		constexpr Invoker( Invoker&& a_Other ) noexcept
		{
			Bind( a_Other.m_Function, a_Other.m_Instance );
			a_Other.Unbind();
		}

		// Free Function Assignment Operator
		constexpr Invoker& operator=( FunctionType* a_Function ) noexcept
		{
			Bind( a_Function );
			return *this;
		}

		// Lambda Assignment Operator
		template<typename _Functor> requires
			std::is_invocable_r_v<ReturnType, _Functor, _Args...> // _Functor must be invocable with the same arguments and return type as the function
			&& !std::is_member_pointer_v<_Functor>                // _Functor must not be a member function pointer
			&& std::is_stateless_v<_Functor>                      // _Functor must be a static lambda (no captures)
		constexpr Invoker& operator=( _Functor a_Functor ) noexcept
		{
			Bind<a_Functor>();
			return *this;
		}

		// Copy Assignment Operator
		constexpr Invoker& operator=( const Invoker& a_Other ) noexcept
		{
			if ( this != &a_Other )
			{
				Bind( a_Other.m_Function, a_Other.m_Instance );
			}
			return *this;
		}

		// Move Assignment Operator
		constexpr Invoker& operator=( Invoker&& a_Other ) noexcept
		{
			if ( this != &a_Other )
			{
				Bind( a_Other.m_Function, a_Other.m_Instance );
				a_Other.Unbind();
			}
			return *this;
		}

		// Connects a free function or an unbound member function to the invoker.
		// _Function: The address of the function to bind. Example: &MyClass::MyFunction
		template<auto _Function>
		void Bind() noexcept
		{
			m_Instance = nullptr;

			if constexpr ( std::is_invocable_r_v<ReturnType, decltype(_Function), _Args...> )
			{
				m_Function = []( const void*, _Args... a_Args ) -> ReturnType
					{
						return ReturnType( std::invoke( _Function, std::forward<_Args>( a_Args )... ) );
					};
			}
			else if constexpr ( std::is_member_pointer_v<decltype(_Function)> )
			{
				static_assert(std::_Always_false, "Member function pointers must be bound to an instance.");
			}
			else
			{
				static_assert(std::_Always_false, "Unknown function type.");
			}
		}

		template<auto _Function, typename _InstanceType>
		constexpr void Bind( _InstanceType& a_Instance ) noexcept
		{
			m_Instance = &a_Instance;
			if constexpr ( std::is_invocable_r_v<ReturnType, decltype(_Function), _InstanceType*, _Args...> )
			{
				m_Function = []( const void* a_Instance, _Args... a_Args ) -> ReturnType
					{
						return (static_cast<_InstanceType*>(const_cast<void*>(a_Instance))->*_Function)(a_Args...);
					};
			}
			else
			{
				static_assert(std::_Always_false, "Unknown function type.");
			}
		}

		constexpr void Bind( FunctionType* a_Function, const void* a_Instance = nullptr ) noexcept
		{
			m_Function = a_Function;
			m_Instance = a_Instance;
		}

		constexpr void Unbind() noexcept
		{
			m_Function = nullptr;
			m_Instance = nullptr;
		}

		constexpr bool Valid() const noexcept
		{
			return m_Function != nullptr;
		}

		constexpr const void* Instance() const noexcept
		{
			return m_Instance;
		}

		constexpr FunctionType* Function() const noexcept
		{
			return m_Function;
		}

		constexpr ReturnType Invoke( _Args... a_Args ) const
		{
			if ( !std::is_constant_evaluated() )
			{
				ENSURE_LOG( m_Function != nullptr, "Attempting to invoke an unbound function." );
			}

			return m_Function( m_Instance, a_Args... );
		}

		constexpr operator bool() const
		{
			return Valid();
		}

		constexpr ReturnType operator()( _Args... a_Args ) const
		{
			return Invoke( a_Args... );
		}

	private:
		const void* m_Instance = nullptr;
		FunctionType* m_Function = nullptr;
	};

	//==============================================================
	// Basic Multicast
	//	An object that can store and invoke multiple invokables via Broadcast.
	template<typename _Invokable, typename _Return, typename... _Args>
	class BasicMulticast
	{
	public:
		using InvokableType = _Invokable;
		using ReturnType = _Return;
		using Handle = uint32_t;

		static constexpr Handle c_InvalidHandle = std::numeric_limits<Handle>::max();

		// Will register the invokable and return a handle to it that can be used to remove it later (if needed).
		Handle Add( const InvokableType& a_Invokable )
		{
			const Handle handle = m_NextHandle++;

			m_HandleToIndex[handle] = m_Invokables.Size();
			m_Invokables.EmplaceBack( a_Invokable );
			m_Handles.EmplaceBack( handle );

			return handle;
		}

		// Will register the invokable and return a handle to it that can be used to remove it later (if needed).
		Handle Add( InvokableType&& a_Invokable )
		{
			const Handle handle = m_NextHandle++;

			m_HandleToIndex[handle] = m_Invokables.Size();
			m_Invokables.EmplaceBack( std::move( a_Invokable ) );
			m_Handles.EmplaceBack( handle );

			return handle;
		}

		// Will remove the invokable with the given handle and returns whether it was successful.
		bool Remove( Handle a_Handle )
		{
			auto it = m_HandleToIndex.find( a_Handle );
			if ( it == m_HandleToIndex.end() )
				return false;

			const size_t index = it->second;

			m_Invokables.Erase( m_Invokables.Begin() + index );
			m_Handles.Erase( m_Handles.Begin() + index );
			m_HandleToIndex.erase( it );

			// Update the indices of the handles after the removed handle
			for ( size_t i = index; i < m_Handles.Size(); ++i )
			{
				m_HandleToIndex[m_Handles[i]] = i;
			}

			return true;
		}

		// Resets and clears all invokables.
		// WARNING: Previous handles will be invalidated.
		void Clear()
		{
			m_Invokables.Clear();
			m_Handles.Clear();
			m_HandleToIndex.clear();
			m_NextHandle = 0;
		}

		// Returns the number of invokables.
		size_t Size() const
		{
			return m_Invokables.Size();
		}

		// Invokes all invokables with the given arguments.
		template<typename... _Args> requires std::is_invocable_v<InvokableType, _Args...>
		void Broadcast( _Args&&... a_Args )
		{
			for ( auto& invokable : m_Invokables )
			{
				invokable( std::forward<_Args>( a_Args )... );
			}
		}

		// Invokes all invokables with the given arguments.
		template<typename... _Args> requires std::is_invocable_v<InvokableType, _Args...>
		void Broadcast( _Args&&... a_Args ) const
		{
			for ( const auto& invokable : m_Invokables )
			{
				invokable( std::forward<_Args>( a_Args )... );
			}
		}

		// Will broadcast and invoke the collector with each return value of the invokables.
		template<typename _Func, typename... _Args> requires std::is_invocable_v<_Func, ReturnType>&& std::is_invocable_v<InvokableType, _Args...>
		void Broadcast( _Func&& a_Collector, _Args&&... a_Args )
		{
			for ( auto& invokable : m_Invokables )
			{
				a_Collector( invokable( std::forward<_Args>( a_Args )... ) );
			}
		}

		// Will attempt to retrieve the invokable with the given handle.
		// Returns a pointer to the invokable if it was found, nullptr otherwise.
		InvokableType* FindInvokable( Handle a_Handle )
		{
			auto it = m_HandleToIndex.find( a_Handle );
			if ( it == m_HandleToIndex.end() )
				return nullptr;
			const size_t index = it->second;
			return &m_Invokables[index];
		}

		// Will invoke a single invokable with the given handle and arguments.
		// Returns whether the invocation was successful.
		template<typename... _Args> requires std::is_invocable_v<InvokableType, _Args...>
		bool Invoke( Handle a_Handle, _Args&&... a_Args )
		{
			auto it = m_HandleToIndex.find( a_Handle );
			if ( it == m_HandleToIndex.end() )
				return false;
			const size_t index = it->second;
			m_Invokables[index]( std::forward<_Args>( a_Args )... );
			return true;
		}

	private:
		Array<InvokableType> m_Invokables{};
		Array<Handle> m_Handles{};
		UnorderedMap<Handle, size_t> m_HandleToIndex{};
		Handle m_NextHandle = 0;
	};



	// Specialization of BasicMulticast for invokers
	template<typename>
	class MulticastInvoker;
	template<typename _Return, typename... _Args>
	class MulticastInvoker<_Return( _Args... )> : public BasicMulticast<Invoker<_Return( _Args... )>, _Return, _Args...> {};
	// Matches BasicMulticast<Invoker<_Return( _Args... )>>::Handle
	using InvokerHandle = uint32_t;
}