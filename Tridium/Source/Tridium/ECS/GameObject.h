#pragma once
#include "EntityComponentSystem.h"
class Scene;

// TEMP ?
#include <Tridium/Core/Application.h>

namespace Tridium {

	typedef entt::entity EntityID;

	class GameObject
	{
	public:
		GameObject( EntityID id, const std::string& name = std::string() );


		template <typename T, typename... Args>
		T& AddComponent( Args&&... args );

		template <typename T>
		T& GetComponent();

		template <typename T>
		bool HasComponent();

	private:
		EntityID m_ID;
	};

#pragma region Game Object Template Definitions

	template <typename T, typename... Args>
	T& GameObject::AddComponent( Args&&... args )
	{
		TE_CORE_ASSERT( !HasComponent<T>(), "GameObject already this component!" )
		return Application::GetScene().m_Registry.emplace<T>( m_ID, std::forward<Args>( args )... );
	}

	template <typename T>
	T& GameObject::GetComponent()
	{
		TE_CORE_ASSERT( HasComponent<T>(), "GameObject does not have this component!" );
		return Application::GetScene().m_Registry.get<T>( m_ID );
	}

	template <typename T>
	bool GameObject::HasComponent()
	{
		return Application::GetScene().m_Registry.has<T>( m_ID );
	}

#pragma endregion

}