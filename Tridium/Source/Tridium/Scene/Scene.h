#pragma once
#include "entt.hpp"

namespace Tridium {
	typedef entt::entity EntityID;

	class Camera;
	class CameraComponent;

	class Scene
	{
		friend class GameObject;

	public:
		Scene();
		~Scene();

		void OnBegin();
		void OnUpdate();
		void Render( const Camera& camera, const Matrix4& viewMatrix );
		void OnEnd();

		GameObject InstantiateGameObject( const std::string& name = "GameObject" );
		auto& GetRegistry() { return m_Registry; }
		void Clear();

		void SetPaused( bool newPaused ) { m_Paused = newPaused; }
		bool IsPaused() const { return m_Paused; }
		CameraComponent* GetMainCamera();
		void SetMainCamera( const EntityID& a_Camera ) { m_MainCamera = a_Camera; }

	private:
		entt::registry m_Registry;
		bool m_Paused = false;

		EntityID m_MainCamera;
	};

#pragma region Scene Template Definitions


#pragma endregion

}