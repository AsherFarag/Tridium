#pragma once
#include "entt.hpp"
#include <Tridium/Asset/Asset.h>
#include <Tridium/Rendering/Lights.h>
#include <Tridium/Rendering/SceneRenderer.h>
#include <Tridium/Physics/PhysicsScene.h>
#include "SceneSystem.h"

#include <Tridium/Debug/DebugDrawer.h>

namespace Tridium {
	using EntityIDType = entt::id_type;
	using EntityID = entt::entity;
	static constexpr EntityID INVALID_ENTITY_ID = entt::null;

	class Camera;
	class CameraComponent;
	class EnvironmentMap;

	struct SceneEnvironment
	{
		struct {
			CubeMapHandle EnvironmentMapHandle;
			SharedPtr<EnvironmentMap> EnvironmentMap;
			float Exposure = 1.0f;
			float Gamma = 2.2f;
			float Blur = 0.0f;
			float Intensity = 1.0f;
			Vector3 RotationEular = { 0.0f, 0.0f, 0.0f };
		} HDRI;
	};

	class Scene final : public Asset
	{
		using SystemStorage = std::unordered_map<size_t, SharedPtr<ISceneSystem>>;
	public:
		Scene( const std::string& a_Name = "Untitled");
		Scene( const Scene& a_Other );
		~Scene();

		void OnBegin();
		void OnUpdate();
		void OnEnd();

		inline const std::string& GetName() const { return m_Name; }
		inline void SetName( const std::string& a_Name ) { m_Name = a_Name; }
		inline auto& GetRegistry() { return m_Registry; }
		inline auto& GetRegistry() const { return m_Registry; }
		inline const auto& GetPhysicsScene() { return m_PhysicsScene; }

		void SetPaused( bool a_NewPaused ) { m_Paused = a_NewPaused; }
		bool IsPaused() const { return m_Paused; }

		bool IsRunning() const { return m_IsRunning; }

		CameraComponent* GetMainCamera();
		EntityID GetMainCameraGameObject() const { return m_MainCamera; }
		void SetMainCamera( const EntityID& a_Camera ) { m_MainCamera = a_Camera; }
		void Clear();

		SceneEnvironment& GetSceneEnvironment() { return m_SceneEnvironment; }
		const SceneEnvironment& GetSceneEnvironment() const { return m_SceneEnvironment; }

		SceneRenderer& GetSceneRenderer() { return m_SceneRenderer; }

		// - Systems -

		void InitSystems();
		void ShutdownSystems();
		void SendSceneEvent( const SceneEventPayload& a_EventPayload );

		template <typename T, typename... Args>
		SharedPtr<T> AddSystem( Args&&... a_Args );

		template <typename T>
		SharedPtr<T> GetSystem();

		// - Physics -

		RayCastResult CastRay( const Vector3& a_Start, const Vector3& a_End,
			ERayCastChannel a_RayCastChannel, const PhysicsBodyFilter& a_BodyFilter = {},
			bool a_DrawDebug = false, 
			Debug::EDrawDuration a_DrawDurationType = Debug::EDrawDuration::OneFrame, float a_DebugDrawDuration = 0.0f,
			Color a_DebugLineColor = Debug::Colors::Red, Color a_DebugHitColor = Debug::Colors::Green ) const;

		// - GameObjects - 
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

	private:
		template <typename T>
		void InitComponent( T& a_Component ) {}

	private:
		std::string m_Name;
		entt::registry m_Registry;
		SceneEnvironment m_SceneEnvironment;
		SystemStorage m_Systems;

		// ========================

		bool m_Paused = false;
		bool m_IsRunning = false;
		bool m_HasBegunPlay = false;
		EntityID m_MainCamera;

		SceneRenderer m_SceneRenderer;
		SharedPtr<PhysicsScene> m_PhysicsScene;

		friend SceneRenderer;
		friend class GameObject;
	};
}

#include "Scene.inl"