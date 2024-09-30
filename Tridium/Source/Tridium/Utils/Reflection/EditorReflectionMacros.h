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


    void DrawAllDataProperties(
        const MetaType& a_Meta,
        MetaAny& a_Handle,
		MetaIDType a_PropertyID,
        auto a_OverrideFlag = ::Tridium::Refl::EPropertyFlag::None )
    {
        using enum ::Tridium::Refl::EPropertyFlag;

        for ( auto&& [id, data] : a_Meta.data() )
        {
            auto drawFuncProp = data.type().prop( ::Tridium::Editor::Internal::DrawPropFuncID );
            if ( !drawFuncProp )
                continue;

            auto drawFunc = drawFuncProp.value().cast<::Tridium::Editor::Internal::DrawPropFunc>();

            // Draw property as EditAnywhere
            if ( HasFlag( data.propFlags(), EditAnywhere ) 
                && a_OverrideFlag != VisibleAnywhere )
            {
                drawFunc( data.name().c_str(), a_Handle, id, static_cast<::Tridium::Refl::PropertyFlags>( EditAnywhere ));
            }
			// Draw property as VisibleAnywhere
            else if ( HasFlag( data.propFlags(), VisibleAnywhere )
                || a_OverrideFlag == VisibleAnywhere )
            {
                drawFunc( data.name().c_str(), a_Handle, id, static_cast<::Tridium::Refl::PropertyFlags>( VisibleAnywhere ) );
            }
        }
    }

    template <typename T>
    void DrawClassAsProperty(
        const char* a_Name,
        MetaAny& a_Handle,
        MetaIDType a_PropertyID,
        ::Tridium::Refl::PropertyFlags a_Flags )
    {
        using enum ::Tridium::Refl::EPropertyFlag;

        auto meta = entt::resolve<T>();

		EPropertyFlag overrideFlag = HasFlag(a_Flags, VisibleAnywhere) ? VisibleAnywhere : EPropertyFlag::None;

		if ( ImGui::TreeNode( a_Name ) )
		{
            for ( auto&& [id, base] : meta.base() )
            {
                DrawAllDataProperties( base, a_Handle, a_PropertyID, overrideFlag );
            }

            DrawAllDataProperties( meta, a_Handle, a_PropertyID, overrideFlag );

			ImGui::TreePop();
		}
    }

}


#pragma endregion

#define _DRAW_PROPERTY_FUNC(Class) \
    meta.prop( ::Tridium::Editor::Internal::DrawPropFuncID, \
		+[]( const char* a_Name, ::Tridium::Refl::MetaAny& a_Handle, ::Tridium::Refl::MetaIDType a_PropertyID, ::Tridium::Refl::PropertyFlags a_Flags ) \
		{ \
			::Tridium::Refl::Internal::DrawClassAsProperty<Class>( a_Name, a_Handle, a_PropertyID, a_Flags ); \
		} );\

#endif // IS_EDITOR
