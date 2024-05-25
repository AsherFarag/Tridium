#include "tripch.h"
#include "Panel.h"
#ifdef IS_EDITOR

namespace Tridium::Editor {

	void Panel::Close()
	{
		m_Owner->DestroyPanel( this );
	}





	PanelStack::~PanelStack()
	{
		for ( Panel* panel : m_Panels )
			delete panel;
	}

	bool PanelStack::PushPanel( Panel* a_Panel )
	{
		if ( a_Panel->m_Owner )
			return false;

		m_Panels.push_back( a_Panel );
		a_Panel->m_Owner = this;
		return true;
	}

	Panel* PanelStack::PullPanel( Panel* a_Panel )
	{
		for ( auto it = m_Panels.begin(); it != m_Panels.end(); it++ )
		{
			if ( *it == a_Panel )
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