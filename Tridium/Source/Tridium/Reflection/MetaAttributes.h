#pragma once
#include <Tridium/Reflection/Meta.h>
#include <Tridium/Containers/StringView.h>
#include <Tridium/Editor/EditorIcons.h>

// Contains built-in meta attributes for reflection.
namespace Tridium::Meta {

	//=================================================================================================
	// Serializable: Marks a field as serializable.
	//=================================================================================================
	struct Serializable : FieldAttribute {};

	//=================================================================================================
	// Scriptable: This property is exposed to the scripting system for read/write access.
	// For a field or function to be scriptable, the type must also be scriptable.
	//=================================================================================================
	struct Scriptable : TypeAttribute, FieldAttribute, FunctionAttribute {};

	//=================================================================================================
	// Script Read Only: This property is exposed to the scripting system for read-only access.
	// Incompatible with: 'Scriptable'.
	//=================================================================================================
	struct ScriptReadOnly : FieldAttribute 
	{
		static constexpr auto ExcludeAttributes = Exclude<Scriptable>();
	};

	//=================================================================================================
	// Require Components: 
	// Specifies that a component requires other components to be present on the same entity.
	// If an entity does not have the required components,
	// it will be created with them when this component is added.
	//=================================================================================================
	template<typename... _Components>
	struct RequireComponents : TypeAttribute
	{
		using Components = std::tuple<_Components...>;
	};

#pragma region Editor Attributes

	//=================================================================================================
	// Editable: This field can be edited in the editor.
	// Incompatible with: 'Visible'.
	//=================================================================================================
	struct Editable : FieldAttribute {};

	//=================================================================================================
	// Visible Anywhere: This field is visible in the editor, but cannot be edited.
	// Incompatible with: 'Editable'.
	//=================================================================================================
	struct Visible : FieldAttribute
	{
		static constexpr auto ExcludeAttributes = Exclude<Editable>();
	};

	//=================================================================================================
	// Display If: Conditionally displays a field in the editor based on the result of a function.
	// The function must be a member function,
	// or a free function that takes in an instance of the containing type and returns a bool.
	//=================================================================================================
	template<auto _TestFunc>
	struct DisplayIf : FieldAttribute, FunctionAttribute
	{
		static constexpr auto TestFunc = _TestFunc;

		static constexpr bool Evaluate( auto& a_Instance )
		{
			if constexpr ( std::is_member_function_pointer_v<decltype(TestFunc)> )
			{
				return ( a_Instance.*TestFunc )();
			}
			else
			{
				return TestFunc( a_Instance );
			}
		}
	};

	//=================================================================================================
	// Display Name: Defines a human-readable display name for a field.
	// If specified, this name will be used in editors instead of the field's actual name.
	//=================================================================================================
	template<StringLiteral _Name>
	struct DisplayName : TypeAttribute, FieldAttribute, FunctionAttribute
	{
		static constexpr StringView Value = _Name;
	};

	//=================================================================================================
	// Icon: Defines an icon that the editor can use for when this type is represented visually.
	// E.g., in a component list.
	//=================================================================================================
	template<StringLiteral _Icon>
	struct Icon : TypeAttribute
	{
		static constexpr StringView Value = _Icon;
	};

	//=================================================================================================
	// Tooltip: Provides a tooltip description for a field or function in the editor.
	//=================================================================================================
	template<StringLiteral _Tooltip>
	struct Tooltip : FieldAttribute, FunctionAttribute
	{
		static constexpr StringView Value = _Tooltip;
	};

	//=================================================================================================
	// Call In Editor: This function can be called from the editor.
	//=================================================================================================
	struct CallInEditor : FunctionAttribute {};

	template<auto _Min, auto _Max, bool _HasMin = true, bool _HasMax = true>
		requires Concepts::Arithmetic<decltype( _Min )> && Concepts::Arithmetic<decltype( _Max )>
	struct Range : FieldAttribute
	{
		using ValueType = decltype( _Min );
		static constexpr auto Min = _Min;
		static constexpr auto Max = _Max;
		static constexpr bool HasMin = _HasMin;
		static constexpr bool HasMax = _HasMax;
	};

	//=================================================================================================
	// Min: Defines a minimum value for a field.
	//=================================================================================================
	template<auto _Min> requires Concepts::Arithmetic<decltype( _Min )>
	using Min = Range<_Min, Math::NumericLimits<decltype( _Min )>::max(), true, false>;

	//=================================================================================================
	// Max: Defines a maximum value for a field.
	//=================================================================================================
	template<auto _Max> requires Concepts::Arithmetic<decltype( _Max )>
	using Max = Range<Math::NumericLimits<decltype( _Max )>::min(), _Max, false, true>;

	//=================================================================================================
	// Text Area: Marks a string field to be displayed as a multi-line text area in the editor.
	//=================================================================================================
	template<uint32_t _MinLines, uint32_t _MaxLines>
	struct TextArea : FieldAttribute
	{
		static constexpr uint32_t MinLines = _MinLines;
		static constexpr uint32_t MaxLines = _MaxLines;
	};

	//=================================================================================================
	// Multiline Text: Marks a string field to be displayed as a multi-line text area in the editor.
	//==================================================================================================
	struct MultilineText : FieldAttribute {};

#pragma endregion

} // namespace Tridium::Meta