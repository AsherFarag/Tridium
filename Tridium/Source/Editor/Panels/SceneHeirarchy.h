#pragma once
#ifdef IS_EDITOR

#include "Panel.h"
#include "InspectorPanel.h"

namespace Tridium { class Scene; }

namespace Tridium::Editor {

	class SceneHeirarchy final : public Panel
	{
	public:
		SceneHeirarchy();
		virtual ~SceneHeirarchy() = default;

		virtual void OnEvent( Event& e ) override;
		virtual void OnImGuiDraw() override;

		GameObject GetSelectedGameObject() const { return m_SelectedGameObject; }

	private:
		bool OnKeyPressed( KeyPressedEvent& e );
		void SetSelectedGameObject( GameObject gameObject );

		void DrawSceneHeirarchy();
		void OpenAddGameObjectPopUp();
		void DrawAddGameObjectPopUp();

		// Scene Heirarchy List
		void DrawSceneNode( GameObject go );

	private:
		Ref<Scene> m_Context = nullptr;
		InspectorPanel* m_Inspector = nullptr;
		GameObject m_SelectedGameObject;
	};
}

#endif // IS_EDITOR
