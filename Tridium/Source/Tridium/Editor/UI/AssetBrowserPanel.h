#pragma once
#include <Tridium/Editor/UI/EditorUI.h>

#if WITH_EDITOR

namespace Tridium {

	class AssetBrowserPanel : public IUIPanel
	{
	public:

	protected:

		//=============================================================================================
		void OnEvent( Event& a_Event ) override;
		void OnUpdate( float a_DeltaTime ) override;
		void OnDraw( StringView a_Name, bool& o_Open ) override;

	protected:

		//=============================================================================================
		void UI_DrawFolderHierarchy();
		void UI_DrawAssetBrowserHeader();
		void UI_DrawAssetItems();

	protected:

		//=============================================================================================
		struct
		{
			bool DirectoryInputActive = false;
			String CurrentDirectory = "Dummy/File/Path";
			ImGuiTextFilter SearchFilter;
		} m_Browser;

		//=============================================================================================
		struct
		{
		} m_FolderHierarchy;

	};

}

#endif // IS_EDITOR