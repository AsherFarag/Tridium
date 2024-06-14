#pragma once

template <typename ReturnT, typename... Args >
class Delegate
{
public:
	Delegate() : m_Functor(nullptr), m_Object(nullptr) {}

	ReturnT operator()(const Args&... args)
	{
		return m_Object->m_Functor( std::forward::<Args>( args )... );
	}

private:
	void* m_Functor;
	void* m_Object;
};