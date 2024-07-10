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
		EditorViewportPanel( const Ref<EditorCamera>& editorCamera );
		virtual ~EditorViewportPanel() = default;

		virtual void OnImGuiDraw() override;

	protected:
		virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

	private:
		void DragDropTarget();
		void DrawManipulationGizmos( const Vector2& viewportBoundsMin, const Vector2& viewportBoundsMax );

		void RenderGameObjectIDs();

		SceneHeirarchyPanel* GetSceneHeirarchy();

	private:
		EGizmoState m_GizmoState = EGizmoState::Translate;
		Ref<EditorCamera> m_EditorCamera;
		SceneHeirarchyPanel* m_SceneHeirarchy = nullptr;

		Ref<Framebuffer> m_IDFBO;
		Ref<Shader> m_GameObjectIDShader;
	};

}

#endif // IS_EDITOR
