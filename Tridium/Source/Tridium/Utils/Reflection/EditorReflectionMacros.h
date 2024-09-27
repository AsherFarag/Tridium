#pragma once

#ifndef IS_EDITOR

#define _DRAW_PROPERTY_FUNC(Class)

#endif // IS_EDITOR

#ifdef IS_EDITOR

#include <Editor/PropertyDrawers.h>
#include <Tridium/Utils/Reflection/MetaRegistry.h>

#pragma region Helpers

#define _REFL_ ::Tridium::Refl::
#define _EDITOR_ ::Tridium::Editor::

#define _HAS_PROPFLAG( Flags, Flag ) ( ( Flags & static_cast<_REFL_ PropertyFlags>( Flag ) ) == static_cast<_REFL_ PropertyFlags>( Flag ) )

#define _DRAW_PROPERTY(Meta, ClassData) \
    for (auto&& [id_unique, data_unique] : Meta.data()) \
    { \
        auto drawFunc = data_unique.type().prop( _EDITOR_ Internal::DrawPropFuncID ); \
        if ( !drawFunc ) continue; \
        auto name = _REFL_ MetaRegistry::GetName( id_unique ); \
        if ( _HAS_PROPFLAG( data_unique.propFlags(), _REFL_ EPropertyFlag::EditAnywhere ) ) \
        { \
            drawFunc.value().cast<_EDITOR_ Internal::DrawPropFunc>()( name, data_unique.get( *ClassData ), static_cast<_REFL_ PropertyFlags>( _REFL_ EPropertyFlag::EditAnywhere ) ); \
        } \
        else if (_HAS_PROPFLAG( data_unique.propFlags(), _REFL_ EPropertyFlag::VisibleAnywhere )) \
        { \
            drawFunc.value().cast<_EDITOR_ Internal::DrawPropFunc>()( name, data_unique.get(*ClassData), static_cast<_REFL_ PropertyFlags>( _REFL_ EPropertyFlag::VisibleAnywhere) ); \
        } \
    }

#pragma endregion

#define _DRAW_PROPERTY_FUNC(Class) \
    meta.prop(_EDITOR_ Internal::DrawPropFuncID, \
              +[]( const entt::meta_handle& a_Handle ) \
              { \
                  auto meta = entt::resolve<Class>(); \
                  auto classData = static_cast<const Class*>(a_Handle->data()); \
                  /* Serialize parent classes */ \
                  for (auto&& [id, base] : meta.base()) \
                  { \
                      _DRAW_PROPERTY( base, classData) \
                  } \
                  _DRAW_PROPERTY(meta, classData) \
              });

#endif // IS_EDITOR
