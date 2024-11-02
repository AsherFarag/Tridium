#pragma once
#ifdef IS_EDITOR
#include "ViewportPanel.h"

namespace Tridium {
	class Shader;
}

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
		EditorViewportPanel( const SharedPtr<EditorCamera>& editorCamera );
		virtual ~EditorViewportPanel() = default;

		virtual void OnImGuiDraw() override;

	protected:
		virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

	private:
		void DragDropTarget();
		void DrawManipulationGizmos( const Vector2& viewportBoundsMin, const Vector2& viewportBoundsMax );

		void RenderGameObjectIDs();
		void RenderSelectionOutline();

		SceneHeirarchyPanel* GetSceneHeirarchy();

	private:
		EGizmoState m_GizmoState = EGizmoState::Translate;
		SharedPtr<EditorCamera> m_EditorCamera;
		SceneHeirarchyPanel* m_SceneHeirarchy = nullptr;

		SharedPtr<Framebuffer> m_IDFBO;
		SharedPtr<Shader> m_GameObjectIDShader;
		SharedPtr<Shader> m_OutlineShader;
	};

}

#endif // IS_EDITOR
