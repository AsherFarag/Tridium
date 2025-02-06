#pragma once
#include <Tridium/ECS/ECS.h>
#include <Tridium/ECS/EntityTicker.h>
#include <Tridium/Asset/Asset.h>
#include <Tridium/Rendering/Lights.h>
#include <Tridium/Rendering/SceneRenderer.h>
#include <Tridium/Physics/PhysicsScene.h>
#include "SceneEnvironment.h"
#include "SceneSystem.h"


#include <Tridium/Debug/DebugDrawer.h>

namespace Tridium {

	// Forward Declarations
	class Camera;
	class CameraComponent;
	class EnvironmentMap;
	// --------------------

	struct SceneState
	{
		bool IsPaused = false;
		bool IsRunning = false;
		bool HasBegunPlay = false;
	};

	class Scene final : public Asset
	{
		using SystemStorage = std::unordered_map<size_t, SharedPtr<ISceneSystem>>;
	public:
		Scene( const std::string& a_Name = "Untitled");
		Scene( const Scene& a_Other );
		~Scene();

		// Called before the first update
		void OnBeginPlay();
		// Called every frame
		void OnUpdate();
		// Called when the scene is destroyed
		void OnEndPlay();

		const std::string& GetName() const { return m_Name; }
		void SetName( const std::string& a_Name ) { m_Name = a_Name; }

		const SceneState& GetState() const { return m_State; }
		void SetPaused( bool a_NewPaused ) { m_State.IsPaused = a_NewPaused; }
		bool IsPaused() const { return m_State.IsPaused; }
		bool IsRunning() const { return m_State.IsRunning; }
		bool HasBegunPlay() const { return m_State.HasBegunPlay; }

		CameraComponent* GetMainCamera();
		GameObject GetMainCameraGameObject() const;
		void SetMainCamera( GameObject a_CameraGameObject );

		SceneEnvironment& GetSceneEnvironment() { return m_SceneEnvironment; }
		const SceneEnvironment& GetSceneEnvironment() const { return m_SceneEnvironment; }
		SceneRenderer& GetSceneRenderer() { return m_SceneRenderer; }

		//////////////////////////////////////////////////////////////////////////
		// Scene Systems
		//////////////////////////////////////////////////////////////////////////

		void InitSystems();
		void ShutdownSystems();
		void SendSceneEvent( const SceneEventPayload& a_EventPayload );

		template <typename T, typename... Args>
		SharedPtr<T> AddSystem( Args&&... a_Args );

		template <typename T>
		SharedPtr<T> GetSystem();

		//////////////////////////////////////////////////////////////////////////
		// Physics
		//////////////////////////////////////////////////////////////////////////

		const auto& GetPhysicsScene() { return m_PhysicsScene; }

		RayCastResult CastRay( const Vector3& a_Start, const Vector3& a_End,
			ERayCastChannel a_RayCastChannel, const PhysicsBodyFilter& a_BodyFilter = {},
			bool a_DrawDebug = false, 
			Debug::EDrawDuration a_DrawDurationType = Debug::EDrawDuration::OneFrame, float a_DebugDrawDuration = 0.0f,
			Color a_DebugLineColor = Debug::Colors::Red, Color a_DebugHitColor = Debug::Colors::Green ) const;

		//////////////////////////////////////////////////////////////////////////
		// ECS
		//////////////////////////////////////////////////////////////////////////

		auto& GetECS() { return m_ECS; }
		auto& GetECS() const { return m_ECS; }
		void Clear();

		template<typename T, typename... _Args>
		void AddEntityTicker( _Args&&... a_Args );

		void DestroyGameObject( GameObject a_GameObject );
		GameObject InstantiateGameObject( const std::string& a_Name = "GameObject" );
		GameObject InstantiateGameObject( GUID a_GUID, const std::string& a_Name = "GameObject" );
		GameObject InstantiateGameObjectFrom( GameObject a_Source );
		void CopyGameObject( GameObject a_Destination, GameObject a_Source );
		bool IsGameObjectValid( GameObject a_GameObject ) const;
		GameObject FindGameObjectByTag( const std::string& a_Tag ) const;
		std::vector<GameObject> FindAllGameObjectsByTag( const std::string& a_Tag ) const;

		template <typename T, typename... Args>
		T& AddComponentToGameObject( GameObject a_GameObject, Args&&... args );

		template <typename T, typename... Args>
		T* TryAddComponentToGameObject( GameObject a_GameObject, Args&&... args );

		template <typename T>
		inline T& GetComponentFromGameObject( GameObject a_GameObject );

		template <typename T>
		inline T* TryGetComponentFromGameObject( GameObject a_GameObject );

		template <typename T>
		inline bool GameObjectHasComponent( GameObject a_GameObject ) const;

		template <typename T>
		inline void RemoveComponentFromGameObject( GameObject a_GameObject );

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
		std::string m_Name;
		SceneEnvironment m_SceneEnvironment;
		SystemStorage m_Systems;

		// ======= ECS =======
		EntityComponentSystem m_ECS;
		std::vector<UniquePtr<IEntityTicker>> m_EntityTickers;
		// ===================

		SceneState m_State;

		EntityID m_MainCamera;

		SceneRenderer m_SceneRenderer;
		SharedPtr<PhysicsScene> m_PhysicsScene;

		friend SceneRenderer;
		friend class GameObject;
	};
}

#include "Scene.inl"