#include "AssetBrowserPanel.h"

#if WITH_EDITOR

namespace Tridium {

	void AssetBrowserPanel::OnEvent( Event& a_Event )
	{
	}

	void AssetBrowserPanel::OnUpdate( float a_DeltaTime )
	{
	}

	void AssetBrowserPanel::OnDraw( StringView a_Name, bool& o_Open )
	{
		if ( ImGui::Begin( a_Name.data(), &o_Open ) )
		{
		}

		ImGui::End();
	}

} // namespace Tridium

#endif // WITH_EDITOR