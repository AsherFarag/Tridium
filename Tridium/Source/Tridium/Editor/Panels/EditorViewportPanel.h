#pragma once
#if IS_EDITOR
#include "ViewportPanel.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/Core/Delegate.h>

namespace Tridium {
	class Shader;
}

namespace Tridium {
	
	class EditorCamera;

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
		virtual ~EditorViewportPanel();

		virtual void OnImGuiDraw() override;

	protected:
		virtual bool OnKeyPressed( KeyPressedEvent& e ) override;

	private:
		void DragDropTarget();
		void DrawManipulationGizmos( const Vector2& viewportBoundsMin, const Vector2& viewportBoundsMax );

		void RenderGameObjectIDs();
		void RenderSelectionOutline();

		void SetSelectedGameObject( GameObject gameObject ) { m_SelectedGameObject = gameObject; }

	private:
		EGizmoState m_GizmoState = EGizmoState::Translate;
		SharedPtr<EditorCamera> m_EditorCamera;

		GameObject m_SelectedGameObject;
		DelegateHandle m_OnGameObjectSelectedHandle;

		SharedPtr<Framebuffer> m_IDFBO;
		SharedPtr<Shader> m_GameObjectIDShader;
		SharedPtr<Shader> m_OutlineShader;
	};

}

#endif // IS_EDITOR
