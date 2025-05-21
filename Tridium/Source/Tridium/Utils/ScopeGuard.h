#pragma once
#include <type_traits>

namespace Tridium {

	template<typename _Func> requires std::is_invocable_v<_Func>
	class ScopeGuard
	{
	public:
		ScopeGuard( _Func&& a_Func )
			: m_Func( std::forward<_Func>( a_Func ) )
		{}

		~ScopeGuard()
		{
			if ( m_Active )
				m_Func();
		}

		void Dismiss()
		{
			m_Active = false;
		}

		void Activate()
		{
			m_Active = true;
		}

		bool IsActive() const
		{
			return m_Active;
		}

	private:
		_Func m_Func;
		bool m_Active = true;
	};

}