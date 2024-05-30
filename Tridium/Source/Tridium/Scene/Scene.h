#pragma once
#include "entt.hpp"

namespace Tridium {

	class Camera;

	class Scene
	{
		friend class GameObject;

	public:
		Scene();
		~Scene();

		void Update();
		void Render( const Camera& camera, const Matrix4& viewMatrix );

		GameObject InstantiateGameObject( const std::string& name = "GameObject" );

		void SetPaused( bool newPaused ) { m_Paused = newPaused; }
		bool IsPaused() const { return m_Paused; }
		auto& GetRegistry() { return m_Registry; }

	private:
		entt::registry m_Registry;

		bool m_Paused = false;
	};

#pragma region Scene Template Definitions


#pragma endregion

}