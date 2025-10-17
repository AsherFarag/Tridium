#pragma once
#include <Tridium/Containers/UnorderedMap.h>
#include <Tridium/Utils/TypeTraits.h>

namespace Tridium {

	template<typename _Value>
	class TypeMap : public UnorderedMap<HashedString, _Value>
	{
	public:

		template<typename T>
		T* find()
		{
			constexpr size_t typeHash = Hashing::TypeHash<T>();
			auto it = UnorderedMap<HashedString, _Value>::find( typeHash );
			if ( it != UnorderedMap<HashedString, _Value>::end() )
			{
				return Cast<T*>( &( it->second ) );
			}

			return nullptr;
		}

	};

} // namespace Tridium