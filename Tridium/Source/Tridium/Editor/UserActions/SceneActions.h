#pragma once
#include <Tridium/Editor/UserActions/UserAction.h>

#if WITH_EDITOR

#include <Tridium/Scene/Prefab.h>
#include <Tridium/Scene/SceneManager.h>

namespace Tridium {

	//=================================================================================================
	enum class EComponentUserActionType
	{
		Add,
		Remove,
		Modify
	};

	template<typename T>
	class ComponentUserAction : public IUserAction
	{
	public:

		//=============================================================================================
		ComponentUserAction( GameObject a_GameObject, EComponentUserActionType a_ActionType )
			: m_SceneID( a_GameObject.Scene()->ID() )
			, m_EntityID( a_GameObject.ID() )
			, m_ActionType( a_ActionType )
		{
			if ( a_ActionType != EComponentUserActionType::Add )
			{
				m_Data = std::move( a_GameObject.Get<T>() );
			}
		}

		//=============================================================================================
		ComponentUserAction( UUID a_SceneID, EntityID a_EntityID, EComponentUserActionType a_ActionType, T& a_Data )
			: m_SceneID( a_SceneID )
			, m_EntityID( a_EntityID )
			, m_ActionType( a_ActionType )
			, m_Data( a_Data )
		{}

		//=============================================================================================
		ComponentUserAction( UUID a_SceneID, EntityID a_EntityID )
			: m_SceneID( a_SceneID )
			, m_EntityID( a_EntityID )
			, m_ActionType( EComponentUserActionType::Add )
		{}

		//=============================================================================================
		~ComponentUserAction() override = default;

		//=============================================================================================
		void Undo() override
		{
			SharedPtr<Scene> scene = SceneManager::Get()->GetSceneByID( m_SceneID );

			if ( !scene )
			{
				LOG( LogCategory::Editor, Warn, "Failed to undo component action: Scene not found." );
				return;
			}

			GameObject gameObject{ *scene, m_EntityID };

			TODO( "Do error handling if the gameobject has been modified in a way we don't know about." );

			switch ( m_ActionType )
			{
				case EComponentUserActionType::Add:
				{
					m_Data = std::move( gameObject.Get<T>() );
					gameObject.Remove<T>();
					m_ActionType = EComponentUserActionType::Remove;
					break;
				}
				case EComponentUserActionType::Remove:
				{
					gameObject.Add<T>( std::move( m_Data ) );
					m_ActionType = EComponentUserActionType::Add;
					break;
				}
				case EComponentUserActionType::Modify:
				{
					if ( !gameObject.Has<T>() )
					{
						LOG( LogCategory::Editor, Warn, "Failed to undo component modification: Component not found on GameObject." );
						return;
					}

					std::swap( m_Data, gameObject.Get<T>() );
				}
			}
		}

		//=============================================================================================
		void Redo() override
		{
			Undo();
		}

		//=============================================================================================
		String ToString() const override
		{
			StringView actionName;
			switch ( m_ActionType )
			{
			case EComponentUserActionType::Add: actionName = "Add"; break;
			case EComponentUserActionType::Remove: actionName = "Remove"; break;
			case EComponentUserActionType::Modify: actionName = "Modify"; break;
			}

			return std::format( "{} Component<{}> on Entity<{}>", actionName, GetTypeName<T>(), Cast<EntityIDType>( m_EntityID ) );
		}

	private:

		//=============================================================================================
		UUID m_SceneID;
		EntityID m_EntityID;
		EComponentUserActionType m_ActionType;
		T m_Data;

	};

	//=================================================================================================
	// User action for creating or deleting a game object.
	//=================================================================================================
	class GameObjectUserAction : public IUserAction
	{
	public:

		//=============================================================================================
		enum class EActionType
		{
			Create,
			Delete
		};

		//=============================================================================================
		GameObjectUserAction( GameObject a_GameObject, EActionType a_ActionType )
		{
			ASSERT( a_GameObject.Valid(), "Invalid GameObject for UserAction" );

			m_SceneID = a_GameObject.Scene()->ID();
			m_ActionType = a_ActionType;
			if ( m_ActionType == EActionType::Delete )
			{
				m_GameObjectData = Prefab::Build( a_GameObject.Scene()->Registry(), a_GameObject.ID() );
				ASSERT( m_GameObjectData.Valid(), "Failed to build Prefab for GameObjectUserAction" );
			}
			else if ( m_ActionType == EActionType::Create )
			{
				m_GameObjectID = a_GameObject.ID();
			}
		}

		//=============================================================================================
		~GameObjectUserAction() override = default;

		//=============================================================================================
		void Undo() override
		{
			SharedPtr<Scene> scene = SceneManager::Get()->GetSceneByID( m_SceneID );

			if ( !scene )
			{
				LOG( LogCategory::Editor, Warn, "Failed to undo game object action: Scene not found." );
				return;
			}

			switch ( m_ActionType )
			{
				case EActionType::Create:
				{
					GameObject gameObject{ *scene, m_GameObjectID };

					if ( gameObject.Valid() )
					{
						m_GameObjectData = Prefab::Build( scene->Registry(), m_GameObjectID );

						if ( auto* hierarchy = gameObject.TryGet<HierarchyComponent>() )
						{
							hierarchy->DestroyChildren( scene->Registry(), m_GameObjectID );
						}

						gameObject.Destroy();
					}

					m_ActionType = EActionType::Delete;

					break;
				}
				case EActionType::Delete:
				{
					EntityID newEntityID = m_GameObjectData.Instantiate( scene->Registry() );
					m_GameObjectID = newEntityID;
					m_ActionType = EActionType::Create;
					break;
				}
			}
		}

		//=============================================================================================
		void Redo() override
		{
			Undo();
		}

	private:

		//=============================================================================================
		UUID m_SceneID;
		EntityID m_GameObjectID;
		Prefab m_GameObjectData;
		EActionType m_ActionType;

	};

} // namespace Tridium

#endif // WITH_EDITOR