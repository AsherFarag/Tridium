#include "tripch.h"
#include "Scene.h"
#include <Tridium/ECS/GameObject.h>

namespace Tridium {

	Scene::Scene()
	{
		struct TransformComponent
		{
			Matrix4 Transform;

			TransformComponent() = default;

			operator Matrix4& ( ) { return Transform; }
			operator const Matrix4& ( ) const { return Transform; }
		};
	}

	Scene::~Scene()
	{
	}

	GameObject Scene::InstantiateGameObject( const std::string& a_Name )
	{
		return GameObject( m_Registry.create(), a_Name );
	}

}