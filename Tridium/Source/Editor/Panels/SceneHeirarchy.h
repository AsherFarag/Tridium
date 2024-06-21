#pragma once
#ifdef IS_EDITOR

#include "Panel.h"
#include <Tridium/ECS/GameObject.h>

namespace Tridium { class Scene; }

namespace Tridium::Editor {

	class SceneHeirarchy final : public Panel
	{
	public:
		SceneHeirarchy() : Panel("Scene Heirarchy") {}
		virtual ~SceneHeirarchy() = default;

		virtual void OnEvent( Event& e ) override;
		virtual void OnImGuiDraw() override;

		GameObject GetSelectedGameObject() const { return m_SelectedGameObject; }

	private:
		bool OnKeyPressed( KeyPressedEvent& e );
		void SetSelectedGameObject( GameObject gameObject );

		void DrawSceneHeirarchy();
		void DrawInspector();

		// Scene Heirarchy List
		void DrawSceneNode( GameObject go );

		// Inspector
		void InspectGameObject( GameObject gameObject );
		template <typename T, typename... Args>
		void AddComponentToSelectedGameObject( Args&&... args )
		{
			if ( m_SelectedGameObject.HasComponent<T>() )
			{
				TE_CORE_ERROR( "{0} already has a [{1}]!", m_SelectedGameObject.GetTag(), typeid( T ).name() );
				return;
			}

			m_SelectedGameObject.AddComponent<T>( std::forward<Args>( args )... );
		}

	private:
		Ref<Scene> m_Context = nullptr;
		GameObject m_SelectedGameObject;
	};
}

#endif // IS_EDITOR
