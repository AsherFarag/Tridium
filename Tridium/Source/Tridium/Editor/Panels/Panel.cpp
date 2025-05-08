#include "tripch.h"
#include "Panel.h"
#if IS_EDITOR

namespace Tridium {

	void Panel::OnEvent( Event& e )
	{
		if ( !m_IsFocused )
			return;

		EventDispatcher dispatcher( e );
		dispatcher.Dispatch<KeyPressedEvent>( TE_BIND_EVENT_FN( Panel::OnKeyPressed, 1 ) );
		dispatcher.Dispatch<MouseButtonPressedEvent>( TE_BIND_EVENT_FN( Panel::OnMouseButtonPressed, 1 ) );
		dispatcher.Dispatch<MouseButtonReleasedEvent>( TE_BIND_EVENT_FN( Panel::OnMouseButtonReleased, 1 ) );
		dispatcher.Dispatch<MouseMovedEvent>( TE_BIND_EVENT_FN( Panel::OnMouseMoved, 1 ) );
	}

	bool Panel::ImGuiBegin( ImGuiWindowFlags a_WindowFlags )
	{
		bool isCollapsed = !ImGui::Begin( m_Name.c_str(), &m_Open, a_WindowFlags );

		return !isCollapsed;
	}

	void Panel::ImGuiEnd()
	{
		ImGui::End();

		if ( !m_Open )
			Close();
	}

	void Panel::Close()
	{
		OnClose();

		ASSERT( m_Owner, "Panel does not have an owner!" );
		m_Owner->DestroyPanel( this );
	}


	PanelStack::~PanelStack()
	{
		for ( auto panel : m_Panels )
			delete panel.second;
	}

	Panel* PanelStack::PullPanel( Panel* a_Panel )
	{
		for ( auto it = m_Panels.begin(); it != m_Panels.end(); it++ )
		{
			if ( it->second == a_Panel )
			{
				m_Panels.erase( it );
				a_Panel->m_Owner = nullptr;
				return a_Panel;
			}
		}
		// a_Panel is not in the Panel Stack
		return nullptr;
	}

	bool PanelStack::DestroyPanel( Panel* a_Panel )
	{
		Panel* panel = PullPanel( a_Panel );
		if ( panel == nullptr )
			return false;

		delete panel;
		return true;
	}
}

#endif