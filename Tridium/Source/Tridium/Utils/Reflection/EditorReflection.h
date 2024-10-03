#pragma once

#ifndef IS_EDITOR

#define _DRAW_PROPERTY_FUNC(Class)

#endif // IS_EDITOR

#ifdef IS_EDITOR

#include <Tridium/Core/Core.h>
#include <Editor/PropertyDrawers.h>
#include <Tridium/Utils/Reflection/MetaRegistry.h>
#include <Tridium/Utils/Reflection/Meta.h>

#pragma region Helpers

#define _REFL_ ::Tridium::Refl::
#define _EDITOR_ ::Tridium::Editor::

namespace Tridium::Refl::Internal {

	// Iterates over all the properties of a meta type and draws them.
	// Returns true if any property was changed, false otherwise.
    bool DrawAllMembersOfMetaClass( const MetaType& a_MetaType, MetaAny& a_Handle, ::Tridium::Refl::EPropertyFlag a_OverrideFlag = ::Tridium::Refl::EPropertyFlag::None );

    template <typename T>
    bool DrawClassAsProperty( const char* a_Name, MetaAny& a_Handle, PropertyFlags a_Flags )
    {
        static const auto metaType = MetaRegistry::ResolveMetaType<T>();
        bool wasChanged = false;
        EPropertyFlag overrideFlag = HasFlag( a_Flags, EPropertyFlag::VisibleAnywhere ) ? EPropertyFlag::VisibleAnywhere : EPropertyFlag::None;

        if ( ImGui::TreeNodeEx( a_Name, ImGuiTreeNodeFlags_Framed ) )
        {
            if ( ImGui::BeginItemTooltip() )
            {
				ImGui::Text( "Class: %s", MetaRegistry::GetCleanTypeName( metaType ) );
                ImGui::EndTooltip();
            }

            for ( auto&& [id, base] : metaType.base() )
            {
                wasChanged |= DrawAllMembersOfMetaClass( base, a_Handle, overrideFlag );
            }

            wasChanged |= DrawAllMembersOfMetaClass( metaType, a_Handle, overrideFlag );

            ImGui::TreePop();
        }

		return wasChanged;
    }
}


#pragma endregion

#define _DRAW_PROPERTY_FUNC(Class)                                                                                     \
    meta.prop( ::Tridium::Editor::Internal::DrawPropFuncID,  														   \
		+[]( const char* a_Name, ::Tridium::Refl::MetaAny& a_Handle, ::Tridium::Refl::PropertyFlags a_Flags )          \
            -> bool                                                                                                    \
		{                                                                                                              \
			 return ::Tridium::Refl::Internal::DrawClassAsProperty<Class>( a_Name, a_Handle, a_Flags );                \
		} );\

#endif // IS_EDITOR
