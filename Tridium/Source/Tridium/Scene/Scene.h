#pragma once
#include "entt.hpp"

namespace Tridium {

	class Scene
	{
		friend class GameObject;

	public:
		Scene();
		~Scene();

		void Update();

		GameObject InstantiateGameObject( const std::string& name = std::string() );

	private:
		entt::registry m_Registry;
	};

#pragma region Scene Template Definitions


#pragma endregion

}