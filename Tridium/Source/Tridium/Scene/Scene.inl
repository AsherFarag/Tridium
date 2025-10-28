#include "Scene.h"

namespace Tridium {

	template<Concepts::Derived<ISceneSystem> T, typename... _Args>
	inline T* Scene::AddSystem( _Args&&... a_Args )
	{
		if ( T* existingSystem = GetSystem<T>() )
		{
			return existingSystem;
		}

		auto system = MakeUnique<T>( std::forward<_Args>( a_Args )... );
		ISceneSystem* systemPtr = system.get();
		system->m_Scene = this;

		m_SceneSystems[Hashing::TypeHash<T>()] = std::move( system );

		if ( m_State.HasInit )
		{
			systemPtr->Init();
		}

		if ( m_State.HasPostInit )
		{
			systemPtr->PostInit();
		}

		if ( m_State.HasBegunPlay )
		{
			systemPtr->OnBeginPlay();
		}

		return Cast<T*>( systemPtr );
	}

	template<Concepts::Derived<ISceneSystem> T, typename... _Args>
	inline T* Scene::AddOrReplaceSystem( _Args&&... a_Args )
	{
		if ( ISceneSystem* existingSystem = GetSystem<T>() )
		{
			if ( m_State.HasBegunPlay )
			{
				existingSystem->OnEndPlay();
			}

			if ( m_State.HasInit )
			{
				existingSystem->Shutdown();
			}
		}

		auto system = MakeUnique<T>( std::forward<_Args>( a_Args )... );
		ISceneSystem* systemPtr = system.get();
		system->m_Scene = this;

		m_SceneSystems[Hashing::TypeHash<T>()] = std::move( system );

		if ( m_State.HasInit )
		{
			systemPtr->Init();
		}

		if ( m_State.HasPostInit )
		{
			systemPtr->PostInit();
		}

		if ( m_State.HasBegunPlay )
		{
			systemPtr->OnBeginPlay();
		}

		return Cast<T*>( systemPtr );
	}

	inline GameObject Scene::CreateEmptyGameObject()
	{
		return GameObject( this, m_Registry.Create() );
	}

	inline GameObject Scene::CreateGameObject( String a_Tag, const Vector3& a_Position )
	{
		GameObject gameObject = CreateEmptyGameObject();
		gameObject.Add<NameComponent>().Name = std::move( a_Tag );
		gameObject.Add<TransformComponent>().SetLocalPosition( a_Position );
		gameObject.Add<HierarchyComponent>();
		return gameObject;
	}

	template<typename... T>
	inline GameObject Scene::CreateGameObject( String a_Tag, const Vector3& a_Position )
	{
		GameObject gameObject = CreateGameObject( std::move( a_Tag ), a_Position );
		( gameObject.Add<T>(), ... );
		return gameObject;
	}

	inline void Scene::DestroyGameObject( GameObject a_GameObject )
	{
		if ( a_GameObject.ID() != NullEntity && a_GameObject.Scene() == this )
		{
			m_Registry.Destroy( a_GameObject.ID() );
		}
	}

} // namespace Tridium