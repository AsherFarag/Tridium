#pragma once
#include <Tridium/Editor/UI/EditorUI.h>

#if WITH_EDITOR

#include <Tridium/Editor/EditorCamera.h>
#include <Tridium/Graphics/Renderer/RenderView.h>
#include <Tridium/Scene/Scene.h>

namespace Tridium {

	class InspectorPanel : public IUIPanel
	{
	public:

	protected:

		//=============================================================================================
		void OnDraw( StringView a_Name, bool& o_Open ) override;

		//=============================================================================================
		void UI_DrawComponents();

	protected:

	};

} // namespace Tridium

#endif // WITH_EDITOR