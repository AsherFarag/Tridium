#pragma once
#include "Component.h"
#include <Tridium/ECS/GameObject.h>

namespace Tridium {

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

		const Scene* GetScene() const { return SceneManager::GetActiveScene(); }

		template<typename T>
		bool HasComponent() const { return GetGameObject().HasComponent<T>(); }

		template<typename T>
		T* TryGetComponent() const { return GetGameObject().TryGetComponent<T>(); }

		template<typename T>
		T& GetComponent() const { return GetGameObject().GetComponent<T>(); }

		template<typename T, typename... _Args>
		T& AddComponent( _Args&&... a_Args ) const { return GetGameObject().AddComponent<T>( std::forward<_Args>( a_Args )... ); }

		template<typename T>
		void RemoveComponent() const { GetGameObject().RemoveComponent<T>(); }
		//================================================================

		//================================================================
		// Component Flags
		EnumFlags<EComponentFlags> Flags() const { return m_Flags; }
		bool HasFlag( EComponentFlags a_Flag ) { return m_Flags.HasFlag( a_Flag ); }
		bool IsActive() { return HasFlag( EComponentFlags::Active ) && GetGameObject().IsActive(); }
		void SetActive( bool a_Active ) { m_Flags.SetFlag( EComponentFlags::Active, a_Active ); }
		bool IsVisible() { return HasFlag( EComponentFlags::Visible ) && GetGameObject().IsVisible(); }
		void SetVisible( bool a_Visible ) { m_Flags.SetFlag( EComponentFlags::Visible, a_Visible ); }
		//================================================================

	private:
		EnumFlags<EComponentFlags> m_Flags = EComponentFlags::Active | EComponentFlags::Visible;
	};

} // namespace Tridium