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

		auto& GetRegistry() const { return m_Registry; }

	private:
		entt::registry m_Registry;
	};

#pragma region Scene Template Definitions


#pragma endregion

}