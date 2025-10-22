#pragma once
#include <string_view>

namespace Tridium {

	//=================================================================================================
	// BasicStringView: Represents a non-owning view of a string.
	// Equivalent to std::basic_string_view.
	//=================================================================================================
	template<typename _Elem, typename _Traits = std::char_traits<_Elem>>
	using BasicStringView = std::basic_string_view<_Elem, _Traits>;

	using StringView = BasicStringView<char>;
	using WStringView = BasicStringView<wchar_t>;

	template<typename T>
	struct Equal<BasicStringView<T>>
	{
		using is_transparent = void;

		bool operator()( const BasicStringView<T>& a_Lhs, const BasicStringView<T>& a_Rhs ) const noexcept
		{
			return a_Lhs == a_Rhs;
		}
	};

	//=================================================================================================
	// BasicStringLiteral: Represents a compile-time string literal.
	// Can be used as a template parameter.
	// E.g., 
	// template<StringLiteral _Name> 
	// struct MyStruct 
	// {
	//		static constexpr StringView Name = _Name;
	// };
	//=================================================================================================
	template<typename _Elem, size_t _Count>
	struct BasicStringLiteral
	{
		_Elem Data[_Count];
		static constexpr size_t Size = _Count - 1; // Exclude null terminator

		constexpr BasicStringLiteral( const _Elem( &a_String )[_Count] )
		{
			for ( size_t i = 0; i < _Count; ++i )
			{
				Data[i] = a_String[i];
			}
		}

		constexpr operator BasicStringView<_Elem>() const noexcept
		{
			return BasicStringView<_Elem>( Data, Size );
		}
	};

	template<size_t _Count>
	using StringLiteral = BasicStringLiteral<char, _Count>;
	template<size_t _Count>
	using WStringLiteral = BasicStringLiteral<wchar_t, _Count>;

} // namespace Tridium