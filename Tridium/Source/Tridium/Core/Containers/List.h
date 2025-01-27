#pragma once
#include <list>

namespace Tridium {

	template <typename T>
	class List
	{
	public:
		List() = default;
		~List() = default;
		void PushBack( const T& a_Value ) { m_Elements.push_back( a_Value ); }
		void PopBack() { m_Elements.pop_back(); }
		void PushFront( const T& a_Value ) { m_Elements.push_front( a_Value ); }
		void PopFront() { m_Elements.pop_front(); }
		void Clear() { m_Elements.clear(); }
		bool IsEmpty() const { return m_Elements.empty(); }
		size_t Size() const { return m_Elements.size(); }
		T& Front() { return m_Elements.front(); }
		const T& Front() const { return m_Elements.front(); }
		T& Back() { return m_Elements.back(); }
		const T& Back() const { return m_Elements.back(); }

		auto Begin() { return m_Elements.begin(); }
		auto End() { return m_Elements.end(); }

		auto begin() { return m_Elements.begin(); }
		auto end() { return m_Elements.end(); }
		auto cbegin() const { return m_Elements.cbegin(); }
		auto cend() const { return m_Elements.cend(); }
		auto rbegin() { return m_Elements.rbegin(); }
		auto rend() { return m_Elements.rend(); }
		auto crbegin() const { return m_Elements.crbegin(); }
		auto crend() const { return m_Elements.crend(); }

	private:
		std::list<T> m_Elements;
	};

} // namespace Tridium