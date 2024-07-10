#pragma once
#include "entt.hpp"
#include <Tridium/Core/Asset.h>

namespace Tridium {
	typedef entt::entity EntityID;

	class Camera;
	class CameraComponent;

	class Scene : public Asset
	{
		friend class GameObject;
		friend class SceneSerializer;

	public:
		Scene( const std::string& name = "Untitled");
		~Scene();

		void OnBegin();
		void OnUpdate();
		void Render( const Camera& camera, const Matrix4& viewMatrix );
		void OnEnd();

		inline const std::string& GetName() const { return m_Name; }

		void SetPaused( bool newPaused ) { m_Paused = newPaused; }
		bool IsPaused() const { return m_Paused; }

		GameObject InstantiateGameObject( const std::string& name = "GameObject" );
		GameObject InstantiateGameObject( GUID guid, const std::string& name = "GameObject" );
		CameraComponent* GetMainCamera();
		void SetMainCamera( const EntityID& a_Camera ) { m_MainCamera = a_Camera; }
		auto& GetRegistry() { return m_Registry; }
		void Clear();

	private:
		std::string m_Name;
		entt::registry m_Registry;
		bool m_Paused = false;

		EntityID m_MainCamera;
	};
}