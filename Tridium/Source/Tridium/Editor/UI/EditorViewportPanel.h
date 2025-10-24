#pragma once
#include <Tridium/Editor/UI/EditorUI.h>

#if WITH_EDITOR

#include <Tridium/Editor/EditorCamera.h>
#include <Tridium/Graphics/Renderer/RenderView.h>

namespace Tridium {

	class EditorViewportPanel : public IUIPanel
	{
	public:

		//=============================================================================================
		enum class EGizmoState
		{
			Translate = 0,
			Rotate,
			Scale,
			UniversalScale
		};

		//=============================================================================================
		EditorViewportPanel();

	protected:

		//=============================================================================================
		void OnEvent( Event& a_Event ) override;
		void OnUpdate( float a_DeltaTime ) override;
		void OnDraw( StringView a_Name, bool& o_Open ) override;

		//=============================================================================================
		void UI_DrawGizmo( const Vector2& a_ViewportBoundsMin, const Vector2& a_ViewportBoundsMax );

		//=============================================================================================
		bool Event_KeyPressed( const KeyPressedEvent& a_Event );

	protected:

		//=============================================================================================
		EGizmoState m_GizmoState = EGizmoState::Translate;
		EditorCamera m_EditorCamera{};
		RenderViewID m_ViewID{};
		Vector2 m_ViewportSize = { 1280, 720 };

	};

}

#endif // IS_EDITOR