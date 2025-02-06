#pragma once
#include <Tridium/Utils/Todo.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/ECS/ECS.h>

namespace Tridium {

	// Forward declarations
	class NativeScriptComponent;

	//================================================================
	// Member function pointers for NativeScriptComponent functions
	template<typename T>
	using OnBeginPlayFunc = void( T::* )( );
	template<typename T>
	using OnUpdateFunc = void( T::* )( float a_DeltaTime );
	template<typename T>
	using OnEndPlayFunc = void( T::* )( );
	//================================================================

	//================================================================
	// Component Concepts
	//  Used to check if a component qualifies for an entity ticker.
	//================================================================
	namespace Concepts {

		template<typename T>
		concept IsDerivedFromNativeScriptComponent = 
			IsBaseOf<NativeScriptComponent, T> && !IsSame<NativeScriptComponent, T>;

		template<typename T>
		concept HasOnBeginPlayFunction =
			requires( T ) { { &T::OnBeginPlay } -> SameAs<OnBeginPlayFunc<T>>; }
			&& IsDerivedFromNativeScriptComponent<T>;


		template<typename T>
		concept HasOnUpdateFunction =
			requires( T ) { { &T::OnUpdate } -> SameAs<OnUpdateFunc<T>>; }
			&& IsDerivedFromNativeScriptComponent<T>;

		template<typename T>
		concept HasOnEndPlayFunction =
			requires( T ) { { &T::OnEndPlay } -> SameAs<OnEndPlayFunc<T>>; }
			&& IsDerivedFromNativeScriptComponent<T>;

	} // namespace Concepts

	//================================================================
	// Entity Ticker Interface
	//	This interface is used to update components in an ECS.
	//	Users can create their own entity tickers by deriving from this class.
	//================================================================
	class IEntityTicker
	{
	public:
		virtual void OnBeginPlay() {}
		virtual void OnUpdate( float a_DeltaTime ) = 0;
		EntityComponentSystem* GetECS() const { return m_ECS; }

		TODO( "This will most likely be removed once proper component initialization exists" )
		virtual UniquePtr<IEntityTicker> Clone() const = 0;

	private:
		EntityComponentSystem* m_ECS = nullptr;
		friend class Scene;
	};


	//================================================================
	// Component Ticker
	//	This class is used to update a component type in an ECS.
	//	If the component has an OnUpdate or OnBeginPlay function, a ComponentTicker will be created for it.
	//	Users can create their own component tickers by declaring a template specialization of this class.
	//  Example:
	//   template<>
	//   class ComponentTicker<MyComponent> : public IEntityTicker
	//   {
	//   public:
	//       void OnUpdate( float a_DeltaTime ) override
	//       {
	//   		auto view = GetECS()->View<MyComponent>();
	//           for ( auto entity : view )
	//   		{
	//   			// Update component
	//   		}
	//       }
	//   };
	//================================================================
	template<typename T>
	class ComponentTicker : public IEntityTicker
	{
	public:
		virtual void OnBeginPlay() override
		{
			if constexpr ( Concepts::HasOnBeginPlayFunction<T> )
			{
				if ( GetECS() == nullptr )
					return;
				auto view = GetECS()->View<T>();
				for ( auto entity : view )
				{
					T& component = view.get<T>( entity );
					component.OnBeginPlay();
				}
			}
		}

		virtual void OnUpdate( float a_DeltaTime ) override
		{
			if constexpr ( Concepts::HasOnUpdateFunction<T> )
			{
				if ( GetECS() == nullptr )
					return;

				auto view = GetECS()->View<T>();
				for ( auto entity : view )
				{
					T& component = view.get<T>( entity );
					component.OnUpdate( a_DeltaTime );
				}
			}
		}

		virtual UniquePtr<IEntityTicker> Clone() const override
		{
			return MakeUnique<ComponentTicker<T>>();
		}

		friend class Scene;
	};


} // namespace Tridium