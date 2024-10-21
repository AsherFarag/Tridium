#pragma once
#include "entt.hpp"
#include <Tridium/Asset/Asset.h>

namespace Tridium {
	typedef entt::entity EntityID;

	class Camera;
	class CameraComponent;

	struct SceneEnvironment
	{
		struct {
			AssetHandle EnvironmentMap;
			float Exposure = 1.0f;
			float Gamma = 2.2f;
			Vector3 RotationEular = { 0.0f, 0.0f, 0.0f };
		} HDRI;
	};

	class Scene : public Asset
	{
		friend class GameObject;

	public:
		Scene( const std::string& name = "Untitled");
		~Scene();

		void OnBegin();
		void OnUpdate();
		void OnEnd();

		inline const std::string& GetName() const { return m_Name; }
		inline void SetName( const std::string& a_Name ) { m_Name = a_Name; }
		inline auto& GetRegistry() { return m_Registry; }
		inline auto& GetRegistry() const { return m_Registry; }

		void SetPaused( bool a_NewPaused ) { m_Paused = a_NewPaused; }
		bool IsPaused() const { return m_Paused; }

		GameObject InstantiateGameObject( const std::string& a_Name = "GameObject" );
		GameObject InstantiateGameObject( GUID a_GUID, const std::string& a_Name = "GameObject" );
		CameraComponent* GetMainCamera();
		void SetMainCamera( const EntityID& a_Camera ) { m_MainCamera = a_Camera; }
		void Clear();

		SceneEnvironment& GetSceneEnvironment() { return m_SceneEnvironment; }

	private:
		std::string m_Name;
		entt::registry m_Registry;
		SceneEnvironment m_SceneEnvironment;
		bool m_Paused = false;
		EntityID m_MainCamera;

		friend class SceneRenderer;
	};
}