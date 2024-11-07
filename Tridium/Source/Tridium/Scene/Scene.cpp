#include "tripch.h"
#include "Scene.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/ECS/Components/Types.h>


namespace Tridium {

	Scene::Scene(const std::string& name)
		: m_Name( name ), m_SceneRenderer(*this)
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::OnBegin()
	{
		// Can call 'OnBegin' on all game objects or something
	}

	void Scene::OnUpdate()
	{
		// Update scriptable objects
		for ( const auto& storage : m_Registry.storage() )
		{
			// Nothing to iterate through, Continue
			if ( storage.second.size() == 0 )
				continue;

			// We know that the data will always be inherited from Component
			void* data = storage.second.value( storage.second[ 0 ] );
			Component* component = (Component*)data;

			// If the type is not inherited from ScriptableComponent, continue 
			if ( dynamic_cast<ScriptableComponent*>( component ) == nullptr )
				continue;

			for ( auto go : storage.second )
				reinterpret_cast<ScriptableComponent*>( storage.second.value( go ) )->OnUpdate();
		}
	}

	void Scene::OnEnd()
	{
	}

	GameObject Scene::InstantiateGameObject( const std::string& a_Name )
	{
		auto go = GameObject( m_Registry.create() );
		AddComponentToGameObject<GUIDComponent>( go );
		AddComponentToGameObject<TagComponent>( go, a_Name );
		AddComponentToGameObject<TransformComponent>( go );
		return go;
	}

	GameObject Scene::InstantiateGameObject( GUID a_GUID, const std::string& a_Name )
	{
		auto go = GameObject( m_Registry.create() );
		AddComponentToGameObject<GUIDComponent>( go, a_GUID );
		AddComponentToGameObject<TagComponent>( go, a_Name );
		AddComponentToGameObject<TransformComponent>( go );
		return go;
	}

	GameObject Scene::InstantiateGameObjectFrom( GameObject a_Source )
	{
		GameObject dst( m_Registry.create() );
		AddComponentToGameObject<GUIDComponent>( dst );
		AddComponentToGameObject<TagComponent>( dst, a_Source.GetTag() );
		TransformComponent& tc = AddComponentToGameObject<TransformComponent>( dst );
		tc.Position = a_Source.GetTransform().Position;
		tc.Rotation = a_Source.GetTransform().Rotation;
		tc.Scale = a_Source.GetTransform().Scale;

		for ( auto [id, storage] : m_Registry.storage() )
		{
			if ( !storage.contains( a_Source ) )
				continue;

			if ( storage.type() == Refl::MetaRegistry::ResolveMetaType<GUIDComponent>().info() )
				continue;

			if ( storage.type() == Refl::MetaRegistry::ResolveMetaType<TagComponent>().info() )
				continue;

			if ( storage.type() == Refl::MetaRegistry::ResolveMetaType<TransformComponent>().info() )
				continue;

			if ( storage.contains(dst) )
				storage.erase( dst );

			storage.push( dst, storage.value( a_Source ) );
		}

		return dst;
	}

	void Scene::CopyGameObject( GameObject a_Destination, GameObject a_Source )
	{

	}

	void Scene::Clear()
	{
		m_Registry.clear();
	}

	CameraComponent* Scene::GetMainCamera()
	{
		// If the scene doesn't have a main camera,
		// get the first camera in the scene
		if ( !m_Registry.valid( m_MainCamera ) )
		{
			auto cameras = m_Registry.view<CameraComponent>();
			// Return nullptr as there are no cameras in the scene
			if ( cameras.empty() )
				return nullptr;

			m_MainCamera = cameras.front();
		}
		return m_Registry.try_get<CameraComponent>( m_MainCamera );
	}

}