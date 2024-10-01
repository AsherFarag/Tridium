#include "tripch.h"
#include "Reflection.h"
#include "EditorReflectionMacros.h"
#include <Editor/PropertyDrawers.h>
#include <Tridium/IO/TextSerializer.h>

namespace Tridium::Refl {

    namespace Internal
    {
        void SerializeMembersOfMetaClass( IO::Archive& a_Archive, const MetaType& a_MetaType, const MetaAny& a_Data )
        {
            for ( auto&& [id, data] : a_MetaType.data() )
            {
                if ( !HasFlag( data.propFlags(), EPropertyFlag::Serialize ) )
                {
                    continue;
                }

                a_Archive << YAML::Key << data.name().c_str() << YAML::Value;

                if ( auto serializeFunc = data.type().prop( YAMLSerializeFuncID ) )
                {
					if ( a_Data.type().is_pointer_like() )
                    {
                        if ( MetaAny ref = *a_Data; ref )
                            serializeFunc.value().cast<SerializeFunc>()( a_Archive, data.get( ref ) );
                    }
                    else
					{
						serializeFunc.value().cast<SerializeFunc>()( a_Archive, data.get( a_Data ) );
					}
                }
                // Print an error message if there is no serialize function for the type.
                else
                {
                    a_Archive << std::string( "Refl Error: No serialize function for type <" ) + data.type().info().name().data(), +">!";
                }
            }
        }

        void SerializeClass( IO::Archive& a_Archive, const MetaAny& a_Data, const MetaType& a_MetaType )
        {
            a_Archive << YAML::BeginMap;

            for ( auto&& [id, baseType] : a_MetaType.base() )
            {
                SerializeMembersOfMetaClass( a_Archive, baseType, a_Data );
            }

            SerializeMembersOfMetaClass( a_Archive, a_MetaType, a_Data );

            a_Archive << YAML::EndMap;
        }
    }

#ifdef IS_EDITOR

    template<typename T>
    void DrawBasicType(
        const char* a_Name,
        MetaAny& a_Handle,
        MetaIDType a_PropertyID,
        PropertyFlags a_Flags )
    {
        ::Tridium::Editor::EDrawPropertyFlags drawFlags 
            = HasFlag( a_Flags, ::Tridium::Refl::EPropertyFlag::EditAnywhere ) 
            ? ::Tridium::Editor::EDrawPropertyFlags::Editable : ::Tridium::Editor::EDrawPropertyFlags::ReadOnly;

        MetaData prop = a_Handle.type().is_pointer_like() ? ( *a_Handle ).type().data( a_PropertyID ) : ( a_Handle ).type().data( a_PropertyID );
		if ( !prop )
        {
			ImGui::Text( "Nested classes and structs are currently not supported." );
            return;
        }

        T value = a_Handle.type().is_pointer_like() ? prop.get( *a_Handle ).cast<T>() : prop.get( a_Handle ).cast<T>();
        if ( ::Tridium::Editor::DrawProperty( a_Name, value, drawFlags ) )
        {
            if ( a_Handle.type().is_pointer_like() ) 
            {
                if ( MetaAny ref = *a_Handle; ref )
                {
                    prop.set( ref, value );
                }
            }
            else 
            {
                prop.set( a_Handle, value );
            }
        }
    }

    // Adds a static draw property lambda to the type
// Converts the PropertyFlags to DrawPropertyFlags. EditAnywhere -> Editable, VisibleAnywhere -> ReadOnly
// Calls the templated DrawProperty function with the value and flags
#define ADD_DRAWPROPERTY_FUNC_TO_TYPE(Type)                                                                                                                      \
            .prop( ::Tridium::Editor::Internal::DrawPropFuncID,                                                                                                  \
                +[]( const char* a_Name, ::Tridium::Refl::MetaAny& a_Handle, ::Tridium::Refl::MetaIDType a_PropertyID, ::Tridium::Refl::PropertyFlags a_Flags )  \
                {                                                                                                                                                \
                    ::Tridium::Refl::DrawBasicType<Type>(a_Name, a_Handle, a_PropertyID, a_Flags);                                                               \
                } )

#else
    #define ADD_DRAWPROPERTY_FUNC_TO_TYPE(Type)
#endif // IS_EDITOR


    void __Internal_InitializeReflection()
    {
        #define ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE(Type)                                                  \
            .prop(Internal::YAMLSerializeFuncID,                                                          \
                +[]( IO::Archive& a_Archive, const ::Tridium::Refl::MetaAny& a_Value )                    \
                {                                                                                         \
                    IO::SerializeToText(a_Archive, a_Value.cast<Type>());                                 \
                })

        #define REFLECT_BASIC_TYPE(Type)          \
			entt::meta<Type>()                    \
			.type(entt::type_hash<Type>::value()) \
			ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE(Type)

		// Basic types
		REFLECT_BASIC_TYPE( bool )ADD_DRAWPROPERTY_FUNC_TO_TYPE( bool );
		REFLECT_BASIC_TYPE( int )ADD_DRAWPROPERTY_FUNC_TO_TYPE( int );
        REFLECT_BASIC_TYPE( int8_t );
        REFLECT_BASIC_TYPE( uint8_t );
        REFLECT_BASIC_TYPE( int16_t );
        REFLECT_BASIC_TYPE( uint16_t );
        REFLECT_BASIC_TYPE( int32_t );
        REFLECT_BASIC_TYPE( uint32_t );
        REFLECT_BASIC_TYPE( int64_t );
        REFLECT_BASIC_TYPE( uint64_t );
        REFLECT_BASIC_TYPE( float )ADD_DRAWPROPERTY_FUNC_TO_TYPE( float );
        REFLECT_BASIC_TYPE( double );
        REFLECT_BASIC_TYPE( char );
        REFLECT_BASIC_TYPE( char* );
        REFLECT_BASIC_TYPE( const char* );
        REFLECT_BASIC_TYPE( unsigned char );
        REFLECT_BASIC_TYPE( short );
        REFLECT_BASIC_TYPE( unsigned short );
        REFLECT_BASIC_TYPE( long );
        REFLECT_BASIC_TYPE( unsigned long );
        REFLECT_BASIC_TYPE( long long );
        REFLECT_BASIC_TYPE( unsigned long long );

        // STD
        REFLECT_BASIC_TYPE( std::string )ADD_DRAWPROPERTY_FUNC_TO_TYPE( std::string );

		// Math
		REFLECT_BASIC_TYPE( Vector2 )ADD_DRAWPROPERTY_FUNC_TO_TYPE( Vector2 );
		REFLECT_BASIC_TYPE( Vector3 )ADD_DRAWPROPERTY_FUNC_TO_TYPE( Vector3 );
		REFLECT_BASIC_TYPE( Vector4 )ADD_DRAWPROPERTY_FUNC_TO_TYPE( Vector4 );

        #undef REFLECT_BASIC_TYPE
        #undef ADD_SERIALIZE_FUNC_TO_TYPE
        #undef ADD_DRAWPROPERTY_FUNC_TO_TYPE
    }

} // namespace Tridium::Reflection