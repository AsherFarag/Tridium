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
		void Render( const CameraComponent& camera );
		void OnEnd();

		GameObject InstantiateGameObject( const std::string& name = "GameObject" );

		void SetPaused( bool newPaused ) { m_Paused = newPaused; }
		bool IsPaused() const { return m_Paused; }
		auto& GetRegistry() { return m_Registry; }

		CameraComponent* GetMainCamera();
		void SetMainCamera( const EntityID& a_Camera ) { m_MainCamera = a_Camera; }

	private:
		entt::registry m_Registry;
		bool m_Paused = false;

		EntityID m_MainCamera;

		// Game
	private:
		void PhysicsTick();
	};

#pragma region Scene Template Definitions


#pragma endregion

}