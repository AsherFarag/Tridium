#pragma once

namespace ImGui {

	enum class EAlignment
	{
		Default = 0,
		Left,
		Right,
		Center
	};

	template<typename _Func>
	struct FunctionScope
	{
		FunctionScope( const _Func& a_Function )
			: Function( a_Function ) {}

		~FunctionScope()
		{
			Function();
		}

		_Func Function;
	};

}