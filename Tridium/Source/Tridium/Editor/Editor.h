#pragma once
#include <Tridium/Editor/Config.h>

#if WITH_EDITOR

#include <Tridium/Application/Application.h>
#include <Tridium/Common/Function.h>
#include <Tridium/Editor/EditorPayload.h>
#include <Tridium/Editor/SelectionManager.h>
#include <Tridium/Editor/UserActions/UserAction.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/UI/UIManager.h>

namespace Tridium {

	DEFINE_TICK_GROUP( EditorTick );
	DEFINE_TICK_GROUP( EditorRender );

	//=================================================================================================
	// Editor:
	//=================================================================================================
	class Editor final : public IAppLayer
	{
	public:

		//=============================================================================================
		Editor();
		~Editor();

		//=============================================================================================
		// Global editor events that can be subscribed and invoked from anywhere in the editor.
		//=============================================================================================
		struct Events
		{
			static MulticastDelegate<void( class GameObject )> OnGameObjectSelected;
			static MulticastDelegate<void( ESelectionContext, const Selectable&, bool )> OnSelectionChanged;
		};

		//=============================================================================================
		static Editor* Get() { return s_Instance; }
		static EditorPayloadManager& GetPayloadManager() { return Get()->m_PayloadManager; }
		static UserActionManager& GetUserActionManager() { return Get()->m_UserActionManager; }
		static UIManager& GetUIManager() { return Get()->m_UIManager; }
		static SelectionManager& GetSelectionManager() { return Get()->m_SelectionManager; }

		//=============================================================================================
		static EScenePlayMode GetScenePlayMode() { return Get()->m_ScenePlayMode; }

	private:

		//=============================================================================================
		static Editor* s_Instance;

		//=============================================================================================
		EditorPayloadManager m_PayloadManager;
		UserActionManager m_UserActionManager;
		UIManager m_UIManager;
		SelectionManager m_SelectionManager;
		bool m_IsTitleBarHovered = false;

		//=============================================================================================
		bool m_ScenePaused = false;
		int32_t m_SceneStepFrames = 0;
		EScenePlayMode m_ScenePlayMode = EScenePlayMode::None;
		EntityComponentRegistry m_SceneSnapshot;

	private:

		void Tick();
		void DrawUI();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent( Event& a_Event ) override;

		void BeginPlay( EScenePlayMode a_PlayMode );
		void Pause( bool a_Pause = true );
		void Step();
		void EndPlay();

		void UI_DrawTitleBar();
		void UI_DrawMenuBar( Vector2 a_Min, Vector2 a_Max );
		void UI_DrawPlayBar( float a_Height );

		bool Event_KeyPressed( const KeyPressedEvent& a_Event );
	};

};

#endif // IS_EDITOR