#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/ECS/GameObject.h>
#include <Tridium/Reflection/ReflectionFwd.h>

namespace Tridium {

	//================================================================
	// COMPONENT_BODY Macro
	//  Used for defining the body of a component and setting up 
	//  common functionality for components.
	//  Usage:
	//   class MyComponent : public Component
	//   {
	//      COMPONENT_BODY( MyComponent, Component );
	//   public:
	//      // Your code here
	//   };
	//================================================================
#define COMPONENT_BODY( _Class, _Parent ) \
	private:                              \
		REFLECT( _Class );	              \
	public:                               \
		using Super = _Parent;            \
		friend class Scene;			      \
		friend class GameObject; 		  

	//================================================================
	// Component
	//  Base class for all components.
	//  Comes with no functionality.
	//  Use for simple data components that do not require scripting
	//  or custom functionality.
	//================================================================
	class Component
	{
		REFLECT( Component );
		friend class Scene;
		friend class GameObject;
	public:
		Component();
		~Component() = default;

		GameObject GetGameObject() const { return m_GameObject; }

	private:
		GameObject m_GameObject;
	};

	namespace EComponentFlags {
		enum Type : uint32_t
		{
			None = 0,
			// If not active, this component will not be updated.
			Active = 1 << 1,
			// If not visible, this component will not be rendered.
			Visible = 1 << 2,
		};
	}

	//================================================================
	// Native Script Component
	//  This component is a base class for all C++ components that require
	//  custom scripting functionality.
	//  You can override scripting functions such as OnBeginPlay, OnUpdate, OnDestroy, etc
	//  WITHOUT them being virtual.
	//  Scripting functions that are not overridden will not be called.
	//  Even if you inherit from a class that overrides a scripting function,
	//  The base class's function will not be called.
	//  To ensure that the base class's function is called, you must call it manually.
	//  Example:
	//   void OnBeginPlay() override
	//   {
	//		Super::OnBeginPlay();
	//      // Your code here
	//   }
	// 
	// NOTE: For a scripting function to be registered, the class must be reflected
	//  and must inherit from NativeScriptComponent.
	//================================================================
	class NativeScriptComponent : public Component
	{
		COMPONENT_BODY( NativeScriptComponent, Component );
	public:
		NativeScriptComponent() = default;
		~NativeScriptComponent() = default;

		//================================================================
		// Scripting Functions
		// Called before the first frame or when this component is created.
		void OnBeginPlay() {}
		// Called every frame.
		void OnUpdate( float a_DeltaTime ) {}
		// Called when this component is destroyed.
		void OnDestroy() {}
		//================================================================

		//================================================================
		// Utility functions that wrap up the GameObject's functions.

		const Scene* GetScene() const { return Application::GetScene().get(); }

		template<typename T>
		bool HasComponent() const { return m_GameObject.HasComponent<T>(); }

		template<typename T>
		T* TryGetComponent() const { return m_GameObject.TryGetComponent<T>(); }

		template<typename T>
		T& GetComponent() const { return m_GameObject.GetComponent<T>(); }

		template<typename T, typename... _Args>
		T& AddComponent( _Args&&... a_Args ) const { return m_GameObject.AddComponent<T>( std::forward<_Args>( a_Args )... ); }

		template<typename T>
		void RemoveComponent() const { m_GameObject.RemoveComponent<T>(); }
		//================================================================

		//================================================================
		// Component Flags
		const EComponentFlags::Type Flags() const { return m_Flags; }
		bool HasFlag( EComponentFlags::Type a_Flag ) { return ( m_Flags & a_Flag ) != 0; }
		bool IsActive() { return HasFlag( EComponentFlags::Active ); }
		void SetActive( bool a_Active ) { m_Flags = EComponentFlags::Type( a_Active ? m_Flags | EComponentFlags::Active : m_Flags & ~EComponentFlags::Active ); }
		bool IsVisible() { return HasFlag( EComponentFlags::Visible ); }
		void SetVisible( bool a_Visible ) { m_Flags = EComponentFlags::Type( a_Visible ? m_Flags | EComponentFlags::Visible : m_Flags & ~EComponentFlags::Visible ); }
		//================================================================

	private:
		EComponentFlags::Type m_Flags{ EComponentFlags::None };
	};

} // namespace Tridium