#pragma once

template <typename Signature>
class Delegate;

template <typename returnT, typename... Args >
class Delegate<returnT(Args...)>
{
	typedef returnT( *Func )( Args... );

public:
	Delegate() {}

	template<typename &func>
	void Bind( Func a_Func ) { m_Func = a_Func; }
	/* Calls the binded function */
	returnT Call( Args&&... a_Args ) { return m_Func( std::forward< Args >( a_Args )... ); }

private:

	Func m_Func;
};

class MultiCastDelegate
{
public:
	void Bind() {}
	/* Calls the binded functions */
	void Broadcast() {}

private:
};