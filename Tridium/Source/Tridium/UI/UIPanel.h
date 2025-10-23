#pragma once
#include <Tridium/UI/Config.h>

#if CONFIG_ENABLE_TOOL_UI

#include <Tridium/Containers/StringView.h>
#include <Tridium/Events/Event.h>

namespace Tridium {

	//=================================================================================================
	// UI Panel Interface: Base class for creating UI objects such as panels, windows, and dialogs.
	//=================================================================================================
	class IUIPanel
	{
	public:

		//=============================================================================================
		IUIPanel() = default;
		virtual ~IUIPanel() = default;

		//=============================================================================================
		virtual void OnEvent( Event& a_Event ) {}

		//=============================================================================================
		// Optional update function called every frame before rendering.
		// Only called if the panel is open.
		virtual void OnUpdate( float a_DeltaTime ) {}

		//=============================================================================================
		virtual void OnDraw( StringView a_Name, bool& o_Open ) = 0;

	};

} // namespace Tridium

#endif // CONFIG_ENABLE_TOOL_UI