#pragma once
#include <Tridium/Reflection/Meta.h>

// Contains built-in meta attributes for reflection.
namespace Tridium::Meta {

	//=================================================================================================
	// Serializable: Marks a field as serializable.
	//=================================================================================================
	struct Serializable : FieldAttribute {};

	//=================================================================================================
	// Scriptable: This property is exposed to the scripting system for read/write access.
	//=================================================================================================
	struct Scriptable : FieldAttribute, FunctionAttribute {};

	//=================================================================================================
	// Script Read Only: This property is exposed to the scripting system for read-only access.
	// Incompatible with: 'Scriptable'.
	//=================================================================================================
	struct ScriptReadOnly : FieldAttribute 
	{
		static constexpr auto ExcludeAttributes = Exclude<Scriptable>();
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
	// Display Name: Defines a human-readable display name for a field.
	// If specified, this name will be used in editors instead of the field's actual name.
	//=================================================================================================
	template<StringLiteral _Name>
	struct DisplayName : FieldAttribute, FunctionAttribute
	{
		static constexpr StringView Value = _Name;
	};

	//=================================================================================================
	// Call In Editor: This function can be called from the editor.
	//=================================================================================================
	struct CallInEditor : FunctionAttribute {};

	//=================================================================================================
	// Min: Defines a minimum value for a field.
	//=================================================================================================
	template<auto _Min> requires Concepts::Arithmetic<decltype( _Min )>
	struct Min : FieldAttribute
	{
		static constexpr auto Value = _Min;
	};

	//=================================================================================================
	// Max: Defines a maximum value for a field.
	//=================================================================================================
	template<auto _Max> requires Concepts::Arithmetic<decltype( _Max )>
	struct Max : FieldAttribute
	{
		static constexpr auto Value = _Max;
	};

#pragma endregion

} // namespace Tridium::Meta