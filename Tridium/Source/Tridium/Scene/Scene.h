#pragma once

#include "entt.hpp"
#include <Tridium/ECS/EntityComponentSystem.h>

namespace Tridium {

	class Scene
	{
		friend class GameObject;

	public:
		Scene();
		~Scene();

		GameObject InstantiateGameObject( const std::string& name = std::string() );

	private:
		entt::registry m_Registry;
	};

#pragma region Scene Template Definitions

#pragma endregion

}