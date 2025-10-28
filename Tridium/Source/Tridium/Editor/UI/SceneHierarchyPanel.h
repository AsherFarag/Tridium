#pragma once
#include <Tridium/Editor/UI/EditorUI.h>

#if WITH_EDITOR

#include <Tridium/Editor/EditorCamera.h>
#include <Tridium/Graphics/Renderer/RenderView.h>
#include <Tridium/Scene/Scene.h>

namespace Tridium {

	class SceneHierarchyPanel : public IUIPanel
	{
	public:

	protected:

		//=============================================================================================
		void OnEvent( Event& a_Event ) override;
		void OnDraw( StringView a_Name, bool& o_Open ) override;

		//=============================================================================================
		void UI_DrawAddButton();
		void UI_DrawSearchBar();
		void UI_DrawHierarchy();
		void UI_DrawHierarchyNode( GameObject a_GameObject );
		void UI_DrawAddGameObjectMenu( GameObject a_Parent );

	protected:

		//=============================================================================================
		ImGuiTextFilter m_SearchFilter;

	};

} // namespace Tridium

#endif // WITH_EDITOR