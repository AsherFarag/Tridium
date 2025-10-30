#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Containers/TypeMap.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Scene/Component.h>
#include <Tridium/ECS/ECS.h>
#include <Tridium/Utils/Log.h>

namespace Tridium {

	class Scene;
	class ISceneSystem;
	struct GameObject;

	//=================================================================================================
	// Scene State:
	//=================================================================================================
	enum class EScenePlayMode
	{
		None,
		Simulate,
		Play,
	};

	//=================================================================================================
	// ISceneSystem: Base class for all scene systems.
	// Scene systems provide global functionality to a scene, they are designed to operate independently
	// and perform actions on views of components.
	//=================================================================================================
	class ISceneSystem
	{
	public:

		//=============================================================================================
		ISceneSystem() = default;
		virtual ~ISceneSystem() = default;

		//=============================================================================================
		// Get the scene that owns this system.
		Scene& OwningScene() const { return *m_Scene; }

		//=============================================================================================
		virtual ESceneTickGroup GetTickGroup() const { return ESceneTickGroup::Default; }

	protected:

		//=============================================================================================
		// Called when a scene is being initialized, just before BeginPlay is called.
		virtual void Init() {}

		//=============================================================================================
		// Called after the scene and all scene systems have been initialized,
		// but before BeginPlay is called.
		virtual void PostInit() {}

		//=============================================================================================
		// Called when the scene is beginning play.
		// NOTE: Not all components may be initialized at this point.
		virtual void OnBeginPlay() {}

		//=============================================================================================
		// Called every frame while the scene is playing.
		virtual void OnUpdate( float a_DeltaTime ) {}

		//=============================================================================================
		// Called when the scene is ending play.
		virtual void OnEndPlay() {}

		//=============================================================================================
		// Called while a scene is being shutdown, 
		// just before the scene and all GameObjects are destroyed.
		virtual void Shutdown() {}

	private:

		//=============================================================================================
		friend class Scene;
		Scene* m_Scene = nullptr;

	};

	//=================================================================================================
	// Component System: An automatically generated scene system for a specific component type T.
	// This system handles lifecycle events for all components of type T that require it.
	// A component system will be created for any component that implements OnBeginPlay, OnUpdate, or OnEndPlay.
	//=================================================================================================
	template<Concepts::Component::HasLifecycleEvents T>
	class ComponentSystem : public ISceneSystem
	{
	public:

		//=============================================================================================
		using ComponentTraits = ComponentTraits<T>;

		//=============================================================================================
		ESceneTickGroup GetTickGroup() const override
		{
			return ComponentTraits::TickGroup;
		}

	protected:

		//=============================================================================================
		virtual void OnBeginPlay() override
		{
			if constexpr ( Concepts::Component::HasOnBeginPlay<T> )
			{
				auto components = OwningScene().Registry().View<T>();
				components.each( []( T& component )
				{
					component.OnBeginPlay();
				} );
			}
		}

		//=============================================================================================
		virtual void OnUpdate(float a_DeltaTime) override
		{
			if constexpr ( Concepts::Component::HasOnUpdate<T> )
			{
				auto components = OwningScene().Registry().View<T>();
				components.each( [a_DeltaTime]( T& component )
				{
					component.OnUpdate( a_DeltaTime );
				} );
			}
		}

		//=============================================================================================
		virtual void OnEndPlay() override
		{
			if constexpr ( Concepts::Component::HasOnEndPlay<T> )
			{
				auto components = OwningScene().Registry().View<T>();
				components.each( []( T& component )
				{
					component.OnEndPlay();
				} );
			}
		}

	};

	//=================================================================================================
	// Scene:
	//=================================================================================================
	DEFINE_ASSET_TYPE( Scene )
	{
	public:

		//=============================================================================================
		bool IsPaused() const { return m_State.IsPaused; }
		void SetPaused( bool a_Paused ) { m_State.IsPaused = a_Paused; }

		//=============================================================================================
		// Returns a reference to the EntityComponentRegistry used by this Scene.
		auto& Registry() { return m_Registry; }

		//=============================================================================================
		// Returns a const reference to the EntityComponentRegistry used by this Scene.
		const auto& Registry() const { return m_Registry; }

		//=============================================================================================
		// If the scene already has a system of type T, this will return the existing system.
		// Otherwise, it will create a new system of type T and add it to the scene.
		template<Concepts::Derived<ISceneSystem> T, typename... _Args>
		T* AddSystem( _Args&&... a_Args );

		//=============================================================================================
		// Adds a system of type T to the scene, replacing any existing system of the same type.
		// Returns the newly added system.
		template<Concepts::Derived<ISceneSystem> T, typename... _Args>
		T* AddOrReplaceSystem( _Args&&... a_Args );

		//=============================================================================================
		// Gets a scene system of the specified type T.
		// Returns nullptr if the system does not exist.
		template<Concepts::Derived<ISceneSystem> T>
		T* GetSystem() 
		{ 
			UniquePtr<ISceneSystem>* systemPtr = m_SceneSystems.find<T>();
			return systemPtr ? Cast<T*>( systemPtr->get() ) : nullptr;
		}

		//=============================================================================================
		// Gets a ComponentSystem of the specified component type T.
		template<Concepts::Component::HasLifecycleEvents T>
		ComponentSystem<T>* GetComponentSystem()
		{
			return GetSystem<ComponentSystem<T>>();
		}

		//=============================================================================================
		// Gives access to the PhysicsSceneSystem.
		// NOTE: This can be nullptr.
		class PhysicsSceneSystem* Physics() { return m_PhysicsSystem; }

		//=============================================================================================
		// Gives access to the RendererSceneSystem.
		// NOTE: This can be nullptr.
		class RendererSceneSystem* Renderer() { return m_RendererSystem; }

		//=============================================================================================
		// Creates a new empty GameObject in the scene and returns it.
		// An empty GameObject has no components.
		GameObject CreateEmptyGameObject();

		//=============================================================================================
		// Creates a new GameObject in the scene and returns it.
		// GameObject's are created with core components by default, (TransformComponent, etc).
		GameObject CreateGameObject( String a_Tag = {}, const Vector3& a_Position = Vector3::Zero() );

		//=============================================================================================
		// Creates a new GameObject with the specified components in the scene and returns it.
		// The game object will be created with core components by default, (TransformComponent, etc).
		template<typename... T>
		GameObject CreateGameObject( String a_Tag = {}, const Vector3& a_Position = Vector3::Zero() );

		//=============================================================================================
		// Destroys the specified GameObject and removes all its components.
		void DestroyGameObject( GameObject a_GameObject );

	protected:

		//=============================================================================================
		friend class Editor;
		friend class Runtime;
		friend class SceneManager;

		//=============================================================================================
		void Init();

		//=============================================================================================
		void OnBeginPlay( EScenePlayMode a_PlayMode );

		//=============================================================================================
		void OnUpdate( float a_DeltaTime );

		//=============================================================================================
		void OnRender( float a_DeltaTime );

		//=============================================================================================
		void TickSceneSystems( ESceneTickGroup a_TickGroup, float a_DeltaTime );

		//=============================================================================================
		void OnEndPlay();

		//=============================================================================================
		void Shutdown();

	protected:

		//=============================================================================================
		// The map that stores all scene systems and their clone functions.
		TypeMap<ISceneSystem, UniquePtr<ISceneSystem>> m_SceneSystems;

		//=============================================================================================
		// Cached pointer to the PhysicsSceneSystem.
		class PhysicsSceneSystem* m_PhysicsSystem = nullptr;

		//=============================================================================================
		// Cached pointer to the RendererSceneSystem.
		class RendererSceneSystem* m_RendererSystem = nullptr;

		//=============================================================================================
		// The registry that manages all entities and components in this Scene.
		EntityComponentRegistry m_Registry;

		//=============================================================================================
		struct
		{
			// Are we updating the scene in Play mode or Simulate mode?
			EScenePlayMode PlayMode = EScenePlayMode::Play;

			// Is the scene currently paused?
			bool IsPaused = false;

			// Has the scene initialized scene systems?
			bool HasInit = false;

			// Has the scene post-initialized scene systems?
			bool HasPostInit = false;

			// Is the scene currently running and BeginPlay has been called?
			bool HasBegunPlay = false;

		} m_State;

		//=============================================================================================
		// Scales the delta time passed to Scene tick functions.
		float m_TimeScale = 1.0f;

	};

} // namespace Tridium

#include "GameObject.h"
#include "Scene.inl"