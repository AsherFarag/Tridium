#include "tripch.h"
#include "Scene.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types.h>

namespace Tridium {

	Scene::Scene()
	{

	}

	Scene::~Scene()
	{
	}

	void Scene::Update()
	{
		auto& view = m_Registry.view<FlyCameraComponent>();

		for ( auto& flyCamera : view )
		{
			m_Registry.get<FlyCameraComponent>( flyCamera ).OnUpdate();
		}
	}

	GameObject Scene::InstantiateGameObject( const std::string& a_Name )
	{
		auto& go = GameObject( m_Registry.create() );
		go.Init( a_Name );
		return go;
	}

}