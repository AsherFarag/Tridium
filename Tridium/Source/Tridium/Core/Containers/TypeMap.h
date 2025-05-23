#pragma once
#include <unordered_map>

namespace Tridium {

	template<typename T, typename _Storage = std::unordered_map<size_t, T>>
	class TypeMap
	{
	public:
		TypeMap() = default;
		~TypeMap() = default;

		_Storage& GetStorage() { return m_Storage; }
		const _Storage& GetStorage() const { return m_Storage; }

	private:
		_Storage m_Storage;
	};

}