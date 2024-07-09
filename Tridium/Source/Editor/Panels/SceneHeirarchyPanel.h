#pragma once
#ifdef IS_EDITOR

#include "Panel.h"
#include "InspectorPanel.h"

namespace Tridium { class Scene; }

namespace Tridium::Editor {

	class SceneHeirarchyPanel final : public Panel
	{
	public:
		SceneHeirarchyPanel();
		virtual ~SceneHeirarchyPanel() = default;

		virtual void OnImGuiDraw() override;

		GameObject GetSelectedGameObject() const { return m_SelectedGameObject; }
		void SetSelectedGameObject( GameObject gameObject );

	private:
		virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

		void DrawSceneHeirarchy();
		void OpenAddPopUp();
		void DrawAddPopUp( GameObject go = {} );
		void DrawSceneNode( GameObject go );

	private:
		Ref<Scene> m_Context = nullptr;
		InspectorPanel* m_Inspector = nullptr;
		GameObject m_SelectedGameObject;
	};
}

#endif // IS_EDITOR
