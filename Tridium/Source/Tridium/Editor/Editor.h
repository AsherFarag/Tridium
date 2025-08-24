#pragma once
#if IS_EDITOR
#include "EditorConfig.h"
#include "EditorLayer.h"
#include "EditorPayload.h"
#include "EditorStyle.h"
#include <Tridium/Core/Application.h>
#include <Tridium/Common/Function.h>
#include "Commands/CommandManager.h"

namespace Tridium {

	// Forward Declarations
	class GameObject;

	//===========================================================================================
	// Editor
	//  The global editor class
	class Editor final : public Application
	{
	public:
		//=======================================================================================
		// Global editor events that can be subscribed and invoked from anywhere in the editor.
		struct Events
		{
			static MulticastDelegate<void( GameObject )> OnGameObjectSelected;
		};

		static Editor* Get() { return Cast<Editor*>( s_Instance ); }
		static EditorPayloadManager& GetPayloadManager() { return Get()->m_PayloadManager; }
		static EditorLayer* GetEditorLayer() { return Get()->m_EditorLayer; }
		static EditorStyle& GetStyle() { return Get()->m_Style; }
		static CommandManager& GetCommandManager() { return Get()->m_CommandManager; }

	private:
		EditorLayer* m_EditorLayer;
		EditorPayloadManager m_PayloadManager;
		EditorStyle m_Style;
		CommandManager m_CommandManager;

	public:
		Editor( CmdLineArgs a_CmdLine );
		~Editor();

		void OnUpdate() override;
	};

};

#endif // IS_EDITOR