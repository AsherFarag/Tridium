#include "tripch.h"
#include "UIManager.h"

#if CONFIG_ENABLE_TOOL_UI

namespace Tridium {

	REGISTER_TICK_GROUP( BeginUIDraw, "Render"_H );
	REGISTER_TICK_GROUP( DrawUI, "BeginUIDraw"_H );
	REGISTER_TICK_GROUP( EndUIDraw, "DrawUI"_H );

	void UIManager::OnEvent( Event& a_Event )
	{
		PROFILE_FUNCTION( ProfilerCategory::UI );

		for ( auto& [name, panelData] : m_Panels )
		{
			if ( panelData.IsOpen && panelData.Panel )
			{
				panelData.Panel->OnEvent( a_Event );

				if ( a_Event.Handled )
				{
					return;
				}
			}
		}
	}

	void UIManager::UpdateUI( float a_DeltaTime )
	{
		PROFILE_FUNCTION( ProfilerCategory::UI );

		for ( auto& [name, panelData] : m_Panels )
		{
			if ( panelData.IsOpen && panelData.Panel )
			{
				panelData.Panel->OnUpdate( a_DeltaTime );
			}
		}
	}

	void UIManager::DrawUI()
	{
		PROFILE_FUNCTION( ProfilerCategory::UI );

		for ( auto it = m_Panels.begin(); it != m_Panels.end(); ++it )
		{
			UIPanelData& panelData = it->second;

			if ( !panelData.IsOpen || !panelData.Panel )
			{
				continue;
			}

			panelData.Panel->OnDraw( panelData.Name, panelData.IsOpen );
		}

	}

}

#endif // CONFIG_ENABLE_TOOL_UI