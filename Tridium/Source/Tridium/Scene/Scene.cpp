#include "tripch.h"
#include "Scene.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Component.h>
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
		for ( const auto& Storage : m_Registry.storage() )
		{
			// Nothing to iterate through, Continue
			if ( Storage.second.size() == 0 )
				continue;

			// We know that the data will always be inherited from Component
			void* data = Storage.second.value( Storage.second[ 0 ] );
			Component* component = (Component*)data;

			// If the type is not inherited from ScriptableComponent, continue 
			if ( dynamic_cast<ScriptableComponent*>( component ) == nullptr )
				continue;

			for ( auto entity : Storage.second )
				static_cast<ScriptableComponent*>( Storage.second.value( entity ) )->OnUpdate();
		}
	}

	GameObject Scene::InstantiateGameObject( const std::string& a_Name )
	{
		auto& go = GameObject( m_Registry.create() );
		go.Init( a_Name );
		return go;
	}

}