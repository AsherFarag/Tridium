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

#define _DRAW_PROPERTY(Meta, ClassData) \
    for (auto&& [id_unique, data_unique] : Meta.data()) \
    { \
        auto drawFunc = data_unique.type().prop( _EDITOR_ Internal::DrawPropFuncID ); \
        if ( !drawFunc ) continue; \
        auto name = data_unique.name().c_str(); \
        if ( HasFlag( data_unique.propFlags(), _REFL_ EPropertyFlag::EditAnywhere ) ) \
        { \
            auto data_unique_any = data_unique.get( *ClassData ); \
            drawFunc.value().cast<_EDITOR_ Internal::DrawPropFunc>()( name, data_unique_any, static_cast<_REFL_ PropertyFlags>( _REFL_ EPropertyFlag::EditAnywhere ) ); \
			data_unique.set( *ClassData, data_unique_any ); \
        } \
        else if (HasFlag( data_unique.propFlags(), _REFL_ EPropertyFlag::VisibleAnywhere )) \
        { \
            auto data_unique_any = data_unique.get( *ClassData ); \
            drawFunc.value().cast<_EDITOR_ Internal::DrawPropFunc>()( name, data_unique_any, static_cast<_REFL_ PropertyFlags>( _REFL_ EPropertyFlag::VisibleAnywhere) ); \
        } \
    }

namespace Tridium::Refl::Internal {


    void DrawAllDataProperties(
        const MetaType& a_Meta,
        MetaHandle a_Handle,
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
                MetaAny dataHandle = data.get( a_Handle->as_ref() );
                drawFunc( data.name().c_str(), dataHandle.as_ref(), static_cast<::Tridium::Refl::PropertyFlags>( EditAnywhere ));
        //        if ( dataHandle.try_cast<int>( ) )
				    //printf( "Data: %d\n", dataHandle.cast<int>( ) );
               // data.set( a_Handle->as_ref(), dataHandle);
            }
			// Draw property as VisibleAnywhere
            else if ( HasFlag( data.propFlags(), VisibleAnywhere )
                || a_OverrideFlag == VisibleAnywhere )
            {
                drawFunc( data.name().c_str(), data.get( a_Handle->as_ref() ), static_cast<::Tridium::Refl::PropertyFlags>( VisibleAnywhere ) );
            }
        }
    }

    template <typename T>
    void DrawClassAsProperty( const char* a_Name, MetaHandle a_Handle, ::Tridium::Refl::PropertyFlags a_Flags )
    {
        using enum ::Tridium::Refl::EPropertyFlag;

        auto meta = entt::resolve<T>();
        auto classData = static_cast<const T*>( a_Handle->data() );

		EPropertyFlag overrideFlag = HasFlag(a_Flags, VisibleAnywhere) ? VisibleAnywhere : EPropertyFlag::None;

		if ( ImGui::TreeNode( a_Name ) )
		{
            for ( auto&& [id, base] : meta.base() )
            {
                DrawAllDataProperties( base, a_Handle->as_ref(), overrideFlag );
            }

            DrawAllDataProperties( meta, a_Handle->as_ref(), overrideFlag );

			ImGui::TreePop();
		}
    }

}


#pragma endregion

#define _DRAW_PROPERTY_FUNC(Class) \
    meta.prop( ::Tridium::Editor::Internal::DrawPropFuncID, \
		+[]( const char* a_Name, ::Tridium::Refl::MetaHandle a_Handle, ::Tridium::Refl::PropertyFlags a_Flags ) \
		{ \
			::Tridium::Refl::Internal::DrawClassAsProperty<Class>( a_Name, a_Handle->as_ref(), a_Flags ); \
		} );\

#endif // IS_EDITOR
