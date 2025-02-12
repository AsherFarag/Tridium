#pragma once

#if !IS_EDITOR

#define _DRAW_PROPERTY_FUNC(Class)
#define _DRAW_ENUM_FUNC(EnumClass)
#define _DECLARE_DEFAULT_ATTRIBUTE_DRAW_PROPERTY
#define _BIND_DEFAULT_ATTRIBUTE_DRAW_PROPERTY

#endif // IS_EDITOR

#if IS_EDITOR

#include <Tridium/Core/Core.h>
#include <Tridium/Editor/PropertyDrawers.h>
#include <Tridium/Reflection/MetaTypes.h>

#pragma region Helpers

namespace Tridium::Refl::Internal {

	// Iterates over all the properties of a meta type and draws them.
	// Returns true if any property was changed, false otherwise.
	bool DrawAllMembersOfMetaClass( const MetaType& a_MetaType, MetaAny& a_Handle, ::Tridium::Refl::EPropertyFlags a_OverrideFlag = ::Tridium::Refl::EPropertyFlags::EPF_None );

	template <typename T>
	bool DrawClassAsProperty( const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags )
	{
		static const MetaType metaType = ResolveMetaType<T>();
		bool wasChanged = false;
		EPropertyFlags overrideFlag = HasFlag( a_Flags, EPropertyFlags::VisibleAnywhere ) ? EPropertyFlags::VisibleAnywhere : EPropertyFlags::EPF_None;

		if ( ImGui::TreeNodeEx( a_Name ) )
		{
			if ( ImGui::BeginItemTooltip() )
			{
				ImGui::Text( "Class: %s", metaType.GetCleanTypeName() );
				ImGui::EndTooltip();
			}

			for ( auto&& [id, base] : metaType.Bases() )
			{
				wasChanged |= DrawAllMembersOfMetaClass( base, a_Handle, overrideFlag );
			}

			wasChanged |= DrawAllMembersOfMetaClass( metaType, a_Handle, overrideFlag );

			ImGui::TreePop();
		}

		return wasChanged;
	}

	template <typename T>
	bool DrawEnumAsProperty( const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags, const std::unordered_map<T, std::string>& a_EnumToString )
	{
		bool wasChanged = false;

		MetaAny handle = a_Handle.type().is_pointer_like() ? *a_Handle : a_Handle;

		T& value = a_Handle.type().is_pointer_like() ?
			( a_Handle.allow_cast<const T*>( ) ?
				*const_cast<T*>( a_Handle.cast<const T*>() ) : *a_Handle.cast<T*>() )
			: a_Handle.cast<T&>();

		const std::string& valueStr = a_EnumToString.at( value );

		if ( ImGui::BeginCombo( a_Name, valueStr.c_str() ) )
		{
			for ( auto&& [enumValue, enumStr] : a_EnumToString )
			{
				bool isSelected = value == enumValue;
				if ( ImGui::Selectable( enumStr.c_str(), isSelected ) )
				{
					value = enumValue;
					wasChanged = true;
				}
				if ( isSelected )
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		return wasChanged;
	}

}


#pragma endregion

#define _DRAW_ENUM_FUNC(EnumClass)                                                                                     \
		+[]( const char* a_Name, ::Tridium::Refl::MetaAny& a_Handle, ::Tridium::Refl::EPropertyFlags a_Flags )          \
			-> bool                                                                                                    \
		{                                                                                                              \
			return ::Tridium::Refl::Internal::DrawEnumAsProperty<EnumClass>( a_Name, a_Handle, a_Flags, s_EnumToString ); \
		}

#define _DECLARE_DEFAULT_ATTRIBUTE_DRAW_PROPERTY  std::optional<::Tridium::Refl::Props::DrawPropertyProp::Type> DrawPropertyProp_Override;

#define _BIND_DEFAULT_ATTRIBUTE_DRAW_PROPERTY \
	{ \
		if ( DrawPropertyProp_Override.has_value() ) { factory.prop( ::Tridium::Refl::Props::DrawPropertyProp::ID, DrawPropertyProp_Override.value() ); } \
		else { factory.prop( ::Tridium::Refl::Props::DrawPropertyProp::ID, +[](const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags) { return ::Tridium::Refl::Internal::DrawClassAsProperty<ClassType>(a_Name, a_Handle, a_Flags); } ); } \
	}

#endif // IS_EDITOR
