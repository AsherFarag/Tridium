#pragma once
#ifdef IS_EDITOR
#include "ViewportPanel.h"

namespace Tridium::Editor {
	
	class EditorCamera;
	class SceneHeirarchyPanel;

	enum class EGizmoState
	{
		Translate = ImGuizmo::TRANSLATE,
		Rotate = ImGuizmo::ROTATE,
		Scale = ImGuizmo::SCALE,
		Universal_Scale = ImGuizmo::SCALEU
	};

	class EditorViewportPanel : public ViewportPanel
	{
	public:
		EditorViewportPanel( EditorCamera& editorCamera );
		virtual ~EditorViewportPanel() = default;

		virtual void OnImGuiDraw() override;

	protected:
		virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

	private:
		void DragDropTarget();
		void DrawManipulationGizmos( const Vector2& viewportBoundsMin, const Vector2& viewportBoundsMax );

		SceneHeirarchyPanel* GetSceneHeirarchy();

	private:
		EGizmoState m_GizmoState = EGizmoState::Translate;
		EditorCamera* m_EditorCamera;
		SceneHeirarchyPanel* m_SceneHeirarchy = nullptr;
	};

}

#endif // IS_EDITOR
