#pragma once
#include <Tridium/Containers/TypeMap.h>

// TEMP
#include <Tridium/ECS/ECS.h>
#include <Tridium/ECS/EntityTicker.h>
#include <Tridium/oldAsset/Asset.h>
#include <Tridium/Graphics/oldRendering/Lights.h>
#include <Tridium/Graphics/oldRendering/SceneRenderer.h>
#include <Tridium/Physics/PhysicsScene.h>
#include "SceneEnvironment.h"
#include "SceneSystem.h"


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
			Color a_DebugLineColor = Color::Red(), Color a_DebugHitColor = Color::Green() ) const;

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
		SharedPtr<PhysicsScene> m_PhysicsScene;

		friend OldSceneRenderer;
		friend class OldGameObject;
	};

#pragma endregion

	struct GameObject;
	class Scene;
	class ISceneSystem;

	namespace Concepts {

		template<typename T>
		concept Component = true;

	} // namespace Concepts

	//=================================================================================================
	// Scene Tick Groups:
	//=================================================================================================
	enum class ESceneTickGroup : uint8_t
	{
		Unknown = 0,

		// Ticking occurs before physics simulation.
		PrePhysics,

		// Ticking occurs after physics simulation.
		PostPhysics,

		// Ticking occurs before rendering work has been submitted.
		PreRender,

		// Ticking occurs after rendering work has been submitted.
		PostRender,

		// Total number of tick groups.
		COUNT,

		Default = PrePhysics
	};

	//=================================================================================================
	// Scene State:
	//=================================================================================================
	enum class ESceneState
	{
		None = 0,
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
		Scene* OwningScene() const { return m_Scene; }

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
		virtual void OnTick( float a_DeltaTime ) {}

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

	#pragma region GameObject Management

		//=============================================================================================
		// Creates a new GameObject in the scene and returns it.
		GameObject InstantiateGameObject();

		//=============================================================================================
		// Creates a new GameObject with the specified components in the scene and returns it.
		template<Concepts::Component... T>
		GameObject InstantiateGameObject();

		//=============================================================================================
		// Destroys the specified GameObject and removes all its components.
		void DestroyGameObject( GameObject a_GameObject );

	#pragma endregion

	protected:

		//=============================================================================================
		friend class SceneManager;

		//=============================================================================================
		void Init();

		//=============================================================================================
		void OnBeginPlay();

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
		TypeMap<UniquePtr<ISceneSystem>> m_SceneSystems;

		//=============================================================================================
		// The registry that manages all entities and components in this Scene.
		EntityComponentRegistry m_Registry;

		//=============================================================================================
		// The current state of the Scene.
		ESceneState m_SceneState = ESceneState::None;
		bool m_IsPaused = false;

		//=============================================================================================
		// Scales the delta time passed to Scene tick functions.
		float m_TimeScale = 1.0f;

	};

	//=================================================================================================
	// GameObject: Simple struct containing an EntityID and a pointer to the owning Scene.
	// Since GameObjects are small, simple POD types, they can be passed around by value.
	//=================================================================================================
	struct GameObject final
	{
	private:

		//=============================================================================================
		Scene* m_Scene = nullptr;
		EntityID m_EntityID = NullEntity;

	public:

		//=============================================================================================
		GameObject() = default;
		GameObject( Scene* a_Scene, EntityID a_EntityID ) : m_Scene( a_Scene ), m_EntityID( a_EntityID ) {}
		~GameObject() = default;

		//=============================================================================================
		operator EntityID() const { return m_EntityID; }

		//=============================================================================================
		// Returns the underlying EntityID of this GameObject.
		[[nodiscard]] EntityID ID() const { return m_EntityID; }

		//=============================================================================================
		// Returns a pointer to the Scene that owns this GameObject.
		[[nodiscard]] Scene* Scene() const { return m_Scene; }

		//=============================================================================================
		// Checks if this GameObject is valid and usable.
		[[nodiscard]] bool Valid() const 
		{ 
			return m_Scene && m_EntityID != NullEntity && m_Scene->Registry().Valid( m_EntityID ); 
		}

		//=============================================================================================
		// Constructs a new component of type T with the passed in arguments,
		// adds it to the GameObject and returns a reference to it.
		// This will assert if the component already exists.
		template<Concepts::Component T, typename... _Args>
		T& Add( _Args&&... a_Args )
		{
			return m_Scene->Registry().Emplace<T>( m_EntityID, std::forward<_Args>( a_Args )... );
		}

		//=============================================================================================
		// Returns references to the components of types T... associated with the specified entity.
		template<Concepts::Component... T>
		[[nodiscard]] decltype( auto ) Get() const
		{
			return m_Scene->Registry().Get<T...>( m_EntityID );
		}

		//=============================================================================================
		// Returns pointers to the components of types T...
		template<Concepts::Component... T>
		[[nodiscard]] decltype( auto ) TryGet() const
		{
			return m_Scene->Registry().TryGet<T...>( m_EntityID );
		}

		//=============================================================================================
		// Checks if the GameObject has the specified components.
		template<Concepts::Component T>
		[[nodiscard]] bool Has() const
		{
			return m_Scene->Registry().AnyOf<T>( m_EntityID );
		}

		//=============================================================================================
		// Checks if the GameObject has any of the specified components.
		template<Concepts::Component... T>
		[[nodiscard]] bool HasAny() const
		{
			return m_Scene->Registry().AnyOf<T...>( m_EntityID );
		}

		//=============================================================================================
		// Checks if the GameObject has all of the specified components.
		template<Concepts::Component... T>
		[[nodiscard]] bool HasAll() const
		{
			return m_Scene->Registry().AllOf<T...>( m_EntityID );
		}

		//=============================================================================================
		// Removes the specified components from the GameObject.
		template<Concepts::Component... T>
		void Remove()
		{
			m_Scene->Registry().Remove<T...>( m_EntityID );
		}

	};

	inline GameObject Scene::InstantiateGameObject()
	{
		return GameObject( this, m_Registry.Create() );
	}

	template<Concepts::Component... T>
	inline GameObject Scene::InstantiateGameObject()
	{
		EntityID entity = m_Registry.Create();
		( m_Registry.Emplace<T>( entity ), ... );
		return GameObject( this, entity );
	}

	inline void Scene::DestroyGameObject( GameObject a_GameObject )
	{
		if ( a_GameObject.ID() != NullEntity && a_GameObject.Scene() == this )
		{
			m_Registry.Destroy( a_GameObject.ID() );
		}
	}

} // namespace Tridium

#include "Scene.inl"