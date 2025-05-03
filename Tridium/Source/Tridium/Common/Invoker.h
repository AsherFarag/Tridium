#pragma once
#include <type_traits>
#include <Tridium/Core/Assert.h>
#include <Tridium/Containers/List.h>

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

		constexpr Invoker() = default;
		constexpr Invoker( FunctionType* a_Function, const void* a_Instance = nullptr )
		{
			Bind( a_Function, a_Instance );
		}

		// Connects a free function or an unbound member function to the invoker.
		// _Function: The address of the function to bind. Example: &MyClass::MyFunction
		template<auto _Function>
		void Bind()
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
		constexpr void Bind( _InstanceType& a_Instance )
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

		constexpr void Bind( FunctionType* a_Function, const void* a_Instance = nullptr )
		{
			m_Function = a_Function;
			m_Instance = a_Instance;
		}

		constexpr void Unbind()
		{
			m_Function = nullptr;
			m_Instance = nullptr;
		}

		constexpr bool Valid() const
		{
			return m_Function != nullptr;
		}

		constexpr const void* Instance() const
		{
			return m_Instance;
		}

		constexpr FunctionType* Function() const
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
	//	An object that can call multiple invokers/functions.
	//	Users are responsible for unbinding instances before destroying them.
	template<typename _Invokable, template<typename T> typename _Container = std::list>
	class BasicMulticast
	{
	public:
		template<typename T>
		using ContainerType = _Container<T>;
		using InvokableType = _Invokable;
		using FunctionType = typename InvokableType::FunctionType;
		using Handle = uint32_t;
		constexpr Handle c_InvalidHandle = std::numeric_limits<Handle>::max();

		BasicMulticast() = default;

		Handle Add( const InvokableType& a_Invokable )
		{
			Handle handle = m_NextHandle++;
			m_Invokables.emplace_back( handle, a_Invokable );
			return handle;
		}

		Handle Add( InvokableType&& a_Invokable )
		{
			Handle handle = m_NextHandle++;
			m_Invokables.emplace_back( handle, std::move( a_Invokable ) );
			return handle;
		}

		void Remove( Handle a_Handle )
		{
			auto it = std::remove_if( m_Invokables.begin(), m_Invokables.end(),
				[&]( const auto& a_Pair ) { return a_Pair.first == a_Handle; } );
			if ( it != m_Invokables.end() )
			{
				m_Invokables.erase( it, m_Invokables.end() );
			}
		}

		void Clear()
		{
			m_Invokables.clear();
			m_NextHandle = 0;
		}

		size_t Size() const
		{
			return m_Invokables.size();
		}

		template<typename... _Args>
		void Broadcast( _Args&&... a_Args )
		{
			for ( const auto& [handle, invokable] : m_Invokables )
			{
				invokable( std::forward<_Args>( a_Args )... );
			}
		}

		template<typename... _Args>
		void Broadcast( _Args&&... a_Args ) const
		{
			for ( const auto& [handle, invokable] : m_Invokables )
			{
				invokable( std::forward<_Args>( a_Args )... );
			}
		}

		BasicMulticast& operator +=( const InvokableType& a_Invokable )
		{
			Add( a_Invokable );
			return *this;
		}

		BasicMulticast& operator +=( InvokableType&& a_Invokable )
		{
			Add( std::move( a_Invokable ) );
			return *this;
		}

	private:
		ContainerType<Pair<Handle, InvokableType>> m_Invokables{};
		Handle m_NextHandle = 0;
	};

	// Matches BasicMulticast<Invoker<_Return( _Args... )>>::Handle
	using InvokerHandle = uint32_t;

	template<typename>
	class MulticastInvoker;

	// Specialization of BasicMulticast for invokers
	template<typename _Return, typename... _Args>
	class MulticastInvoker<_Return(_Args...)> : public BasicMulticast<Invoker<_Return(_Args...)>>
	{
	};
}