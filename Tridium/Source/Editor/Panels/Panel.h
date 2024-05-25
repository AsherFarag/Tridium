#pragma once

#ifdef IS_EDITOR
#include "imgui.h"

namespace Tridium::Editor {

	class PanelStack;

	class Panel
	{
		friend PanelStack;
	public:
		Panel() = default;
		virtual ~Panel() = default;

		virtual void OnImGuiDraw() = 0;

	protected:
		void Close();

	protected:
		PanelStack* m_Owner = nullptr;
		bool m_Open = true;
	};

	// Wrapper for a list of panels
	class PanelStack
	{
	public:
		~PanelStack();

		// Returns true if this panel was successfully added,
		// false if the panel is already in a panel stack.
		bool PushPanel( Panel* a_Panel );

		// Removes the panel from the stack and returns it.
		// Returns nullptr if the panel was not in the stack!
		Panel* PullPanel( Panel* a_Panel );

		// Returns true if the panel was successfully removed and deleted
		bool DestroyPanel( Panel* a_Panel );

		inline auto operator[]( int i ) { return m_Panels[ i ]; }
		inline auto operator[]( const int i ) const { return m_Panels[ i ]; }
		inline auto begin() { return m_Panels.begin(); }
		inline auto end() { return m_Panels.end(); }
		inline auto size() { return m_Panels.size(); }

	private:
		std::vector<Panel*> m_Panels;
	};

}

#endif // IS_EDITOR