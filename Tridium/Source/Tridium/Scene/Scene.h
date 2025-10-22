#pragma once
#include <Tridium/Containers/TypeMap.h>
#include <Tridium/Scene/Component.h>
#include <Tridium/Utils/Log.h>

// TEMP
#include <Tridium/ECS/ECS.h>
#include <Tridium/ECS/EntityTicker.h>
#include <Tridium/oldAsset/Asset.h>
#include <Tridium/Graphics/oldRendering/Lights.h>
#include <Tridium/Graphics/oldRendering/SceneRenderer.h>
#include "SceneEnvironment.h"
#include "SceneSystem.h"
#include <Tridium/Physics/RayCast.h>


#include <Tridium/Debug/DebugDrawer.h>

namespace Tridium {

#pragma region Old

	// Forward Declarations
	class Camera;
	class CameraComponent;
	class EnvironmentMapOld;

	struct SceneState
	{
		bool IsPaused = false;
		bool IsRunning = false;
		bool HasBegunPlay = false;
	};

	class OldScene final : public Asset
	{
		using SystemStorage = std::unordered_map<size_t, SharedPtr<OldISceneSystem>>;
	public:
		OldScene( const String& a_Name = "Untitled");
		OldScene( const OldScene& a_Other );
		~OldScene();

		// Called before the first update
		void OnBeginPlay();
		// Called every frame
		void OnUpdate();
		// Called when the scene is destroyed
		void OnEndPlay();

		const String& GetName() const { return m_Name; }
		void SetName( const String& a_Name ) { m_Name = a_Name; }

		const SceneState& GetState() const { return m_State; }
		void SetPaused( bool a_NewPaused ) { m_State.IsPaused = a_NewPaused; }
		bool IsPaused() const { return m_State.IsPaused; }
		bool IsRunning() const { return m_State.IsRunning; }
		bool HasBegunPlay() const { return m_State.HasBegunPlay; }

		CameraComponent* GetMainCamera();
		OldGameObject GetMainCameraGameObject() const;
		void SetMainCamera( OldGameObject a_CameraGameObject );

		SceneEnvironment& GetSceneEnvironment() { return m_SceneEnvironment; }
		const SceneEnvironment& GetSceneEnvironment() const { return m_SceneEnvironment; }
		OldSceneRenderer& GetSceneRenderer() { return m_SceneRenderer; }

		//////////////////////////////////////////////////////////////////////////
		// Scene Systems
		//////////////////////////////////////////////////////////////////////////

		void InitSystems();
		void ShutdownSystems();
		void SendSceneEvent( const SceneEventPayload& a_EventPayload );

		template <typename T, typename... Args> requires Concepts::IsBaseOf<OldISceneSystem, T>
		SharedPtr<T> AddSystem( Args&&... a_Args );

		template <typename T> requires Concepts::IsBaseOf<OldISceneSystem, T>
		SharedPtr<T> GetSystem();

		//////////////////////////////////////////////////////////////////////////
		// Physics
		//////////////////////////////////////////////////////////////////////////

		const auto& GetPhysicsScene() { return m_PhysicsScene; }

		RayCastResult CastRay( const Vector3& a_Start, const Vector3& a_End,
			ERayCastChannel a_RayCastChannel, const PhysicsBodyFilter& a_BodyFilter = {},
			bool a_DrawDebug = false, 
			Debug::EDrawDuration a_DrawDurationType = Debug::EDrawDuration::OneFrame, float a_DebugDrawDuration = 0.0f,
			Color4 a_DebugLineColor = Color4::Red(), Color4 a_DebugHitColor = Color4::Green() ) const;

		//////////////////////////////////////////////////////////////////////////
		// ECS
		//////////////////////////////////////////////////////////////////////////

		auto& GetECS() { return m_ECS; }
		auto& GetECS() const { return m_ECS; }
		void Clear();

		template<typename T, typename... _Args>
		void AddEntityTicker( _Args&&... a_Args );

		void DestroyGameObject( OldGameObject a_GameObject );
		OldGameObject InstantiateGameObject( const String& a_Name = "GameObject" );
		OldGameObject InstantiateGameObject( GUID a_GUID, const String& a_Name = "GameObject" );
		OldGameObject InstantiateGameObjectFrom( OldGameObject a_Source );
		void CopyGameObject( OldGameObject a_Destination, OldGameObject a_Source );
		bool IsGameObjectValid( OldGameObject a_GameObject ) const;
		OldGameObject FindGameObjectByTag( const String& a_Tag ) const;
		std::vector<OldGameObject> FindAllGameObjectsByTag( const String& a_Tag ) const;

		template <typename T, typename... Args>
		T& AddComponentToGameObject( OldGameObject a_GameObject, Args&&... args );

		template <typename T, typename... Args>
		T* TryAddComponentToGameObject( OldGameObject a_GameObject, Args&&... args );

		template <typename T>
		inline T& GetComponentFromGameObject( OldGameObject a_GameObject );

		template <typename T>
		inline T* TryGetComponentFromGameObject( OldGameObject a_GameObject );

		template <typename T>
		inline bool GameObjectHasComponent( OldGameObject a_GameObject ) const;

		template <typename T>
		inline void RemoveComponentFromGameObject( OldGameObject a_GameObject );

		// === For internal use only ===
		// Registers a component ticker for the specified component type
		// and connects OnComponentCreated and OnComponentDestroyed callbacks
		template <typename T>
		bool __InitComponentType();

	private:
		bool Initialize();

		//////////////////////////////////////////////////////////////////////////
		// ECS
		//////////////////////////////////////////////////////////////////////////

		void InitAllComponentTypes();

		// EnTT Callbacks
		template <typename T>
		void OnComponentCreated( entt::registry& a_Registry, entt::entity a_Entity );
		template <typename T>
		void OnComponentDestroyed( entt::registry& a_Registry, entt::entity a_Entity );

		//////////////////////////////////////////////////////////////////////////

	private:
		String m_Name;
		SceneEnvironment m_SceneEnvironment;
		SystemStorage m_Systems;

		// ======= ECS =======
		OldEntityComponentSystem m_ECS;
		std::vector<UniquePtr<IEntityTicker>> m_EntityTickers;
		// ===================

		SceneState m_State;

		EntityID m_MainCamera;

		OldSceneRenderer m_SceneRenderer;
		SharedPtr<class IPhysicsScene> m_PhysicsScene;

		friend OldSceneRenderer;
		friend class OldGameObject;
	};

#pragma endregion

	class Scene;
	class ISceneSystem;
	struct GameObject;

	//=================================================================================================
	// Scene State:
	//=================================================================================================
	enum class EScenePlayMode
	{
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
		Scene* m_Scene;

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
			if constexpr ( Concepts::HasOnBeginPlayFunction<T> )
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
			if constexpr ( Concepts::HasOnUpdateFunction<T> )
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
			if constexpr ( Concepts::HasOnEndPlayFunction<T> )
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
	class Scene
	{
	public:

		//=============================================================================================
		NON_COPYABLE_OR_MOVABLE( Scene );
		Scene() = default;
		~Scene() = default;

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
		// Creates a new GameObject in the scene and returns it.
		GameObject InstantiateGameObject();

		//=============================================================================================
		// Creates a new GameObject with the specified components in the scene and returns it.
		template<typename... T>
		GameObject InstantiateGameObject();

		//=============================================================================================
		// Destroys the specified GameObject and removes all its components.
		void DestroyGameObject( GameObject a_GameObject );

	protected:

		//=============================================================================================
		friend class SceneManager;

		//=============================================================================================
		void Init();

		//=============================================================================================
		void OnBeginPlay( EScenePlayMode a_PlayMode );

		//=============================================================================================
		void OnTick( float a_DeltaTime );

		//=============================================================================================
		void TickSceneSystems( ESceneTickGroup a_TickGroup, float a_DeltaTime );

		//=============================================================================================
		void OnEndPlay();

		//=============================================================================================
		void Shutdown();

	protected:

		//=============================================================================================
		// The map that stores all scene systems.
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