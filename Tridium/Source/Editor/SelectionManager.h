#pragma once
#ifdef IS_EDITOR
#include <string>
#include <any>

namespace Tridium::Editor {

	class SelectionManager
	{
	public:



		std::any GetSelection( const std::string& a_Type )
		{
			if ( m_Type == a_Type )
			{
				return m_Selection;
			}

			return std::any();
		}

	private:
		std::string m_Type;
		std::any m_Selection;
	};

}

#endif // IS_EDITOR