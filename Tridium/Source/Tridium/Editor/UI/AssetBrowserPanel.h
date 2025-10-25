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

	};

}

#endif // IS_EDITOR