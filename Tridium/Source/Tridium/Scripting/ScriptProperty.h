#pragma once
#include <Tridium/Core/Types.h>
#include <string>
#include <unordered_map>
#include <Tridium/Reflection/MetaTypes.h>

namespace Tridium {

	struct ScriptVariable
	{
		Refl::MetaAny Value;

		ScriptVariable() = default;
		ScriptVariable( const Refl::MetaAny& a_Value )
			: Value( a_Value )
		{
		}

		template<typename T>
		ScriptVariable( const T& a_Value )
		{
			Value = a_Value;
		}

		bool IsValid() const
		{
			return (bool)Value;
		}
	};

	using ScriptVariableMap = std::unordered_map<std::string, ScriptVariable>;
}