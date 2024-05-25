#pragma once
#ifdef IS_EDITOR

#include "Panel.h"
#include <Tridium/ECS/GameObject.h>

namespace Tridium { class Scene; }

namespace Tridium::Editor {

	class SceneHeirarchy final : public Panel
	{
	public:
		SceneHeirarchy();
		virtual ~SceneHeirarchy() = default;

		virtual void OnImGuiDraw() override;

		GameObject GetSelectedGameObject() const { return m_SelectedGameObject; }
		void SetSelectedGameObject( GameObject gameObject ) { m_SelectedGameObject = gameObject; }

	private:
		void DrawSceneHeirarchy();
		void DrawInspector();

		void DrawComponents( GameObject gameObject );

		template <typename T>
		void AddComponentToSelectedGameObject()
		{
			if ( m_SelectedGameObject.HasComponent<T>() )
			{
				TE_CORE_ERROR( "{0} already has a [{1}]!", m_SelectedGameObject.GetTag(), typeid( T ).name() );
				return;
			}

			m_SelectedGameObject.AddComponent<T>();
		}

	private:
		Ref<Scene> m_Context = nullptr;
		GameObject m_SelectedGameObject;
	};
}

#endif // IS_EDITOR
