#pragma once
#include <Tridium/Editor/UserActions/UserAction.h>

#if WITH_EDITOR

#include <Tridium/Scene/SceneManager.h>

namespace Tridium {

	//=============================================================================================
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


} // namespace Tridium

#endif // WITH_EDITOR