#pragma once
#include <Tridium/Containers/UnorderedMap.h>
#include <Tridium/Core/Hash.h>

namespace Tridium {

	template<typename _Key, typename _Value = _Key>
	class TypeMap : public UnorderedMap<HashedString, _Value>
	{
	public:

		template<typename T>
		_Value* find()
		{
			constexpr auto typeHash = Hashing::TypeHash<_Key>();
			auto it = UnorderedMap<HashedString, _Value>::find( typeHash );
			if ( it != UnorderedMap<HashedString, _Value>::end() )
			{
				return &it->second;
			}

			return nullptr;
		}

	};

} // namespace Tridium