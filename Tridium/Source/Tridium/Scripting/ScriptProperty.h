#pragma once
#include <Tridium/Core/Types.h>
#include <string>
#include <unordered_map>

namespace Tridium {

	struct ScriptVariable
	{
		ScriptVariable() = default;

		template<typename T>
		ScriptVariable( const T& a_Value )
		{
		}

		bool IsValid() const
		{
			return false;
		}
	};

	using ScriptVariableMap = std::unordered_map<std::string, ScriptVariable>;
}