#pragma once
#include <Tridium/Editor/Config.h>

#if WITH_EDITOR

#include <Tridium/Editor/EditorPayload.h>
#include <Tridium/Editor/EditorStyle.h>
#include <Tridium/Core/Application.h>
#include <Tridium/Common/Function.h>
#include <Tridium/Editor/Commands/CommandManager.h>
#include <Tridium/UI/UIManager.h>

namespace Tridium {

	class OldGameObject;

	DEFINE_TICK_GROUP( EditorTick );
	DEFINE_TICK_GROUP( EditorRender );

	struct SelectionContext
	{
		GameObject SelectedObject;
	};

	//=================================================================================================
	// Editor:
	//=================================================================================================
	class Editor final : public Layer
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
			static MulticastDelegate<void( OldGameObject )> OnGameObjectSelected;
		};

		//=============================================================================================
		static Editor* Get() { return s_Instance; }
		static EditorPayloadManager& GetPayloadManager() { return Get()->m_PayloadManager; }
		static EditorStyle& GetStyle() { return Get()->m_Style; }
		static CommandManager& GetCommandManager() { return Get()->m_CommandManager; }
		static UIManager& GetUIManager() { return Get()->m_UIManager; }
		static SelectionContext& GetSelectionContext() { return Get()->m_SelectionContext; }

	private:

		//=============================================================================================
		static Editor* s_Instance;

		//=============================================================================================
		EditorPayloadManager m_PayloadManager;
		EditorStyle m_Style;
		CommandManager m_CommandManager;
		UIManager m_UIManager;
		SelectionContext m_SelectionContext;

	private:

		void Tick();
		void DrawUI();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent( Event& a_Event ) override;

		void UI_DrawMenuBar();
		void UI_DrawToolBar();

		bool Event_KeyPressed( const KeyPressedEvent& a_Event );
	};

};

#endif // IS_EDITOR