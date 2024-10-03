#include "tripch.h"
#include "Reflection.h"
#include "EditorReflection.h"
#include <Editor/PropertyDrawers.h>
#include <Tridium/IO/TextSerializer.h>

namespace Tridium::Refl {

    namespace Internal
    {
        void SerializeMembersOfMetaClass( IO::Archive& a_Archive, const MetaType& a_MetaType, const MetaAny& a_Data )
        {
            for ( auto&& [id, metaData] : a_MetaType.data() )
            {
                if ( !HasFlag( metaData.propFlags(), EPropertyFlag::Serialize ) )
                {
                    continue;
                }

                a_Archive << YAML::Key << metaData.name().c_str() << YAML::Value;

				MetaAny memberData = a_Data.type().is_pointer_like() ? metaData.get( *a_Data ) : metaData.get( a_Data );

				// If the type has a serialize function, call it.
                if ( auto serializeFunc = metaData.type().prop( YAMLSerializeFuncID ) )
                {
					serializeFunc.value().cast<SerializeFunc>()( a_Archive, memberData );
                }
                else if ( metaData.type().is_sequence_container() )
				{
					a_Archive << YAML::Flow << YAML::BeginSeq;
					for ( auto&& element : memberData.as_sequence_container() )
					{
						if ( auto serializeFunc = element.type().prop( YAMLSerializeFuncID ) )
						{
							serializeFunc.value().cast<SerializeFunc>()( a_Archive, element );
						}
					}
					a_Archive << YAML::EndSeq;
				}
				// If the type is an associative container, serialize each key-value pair.
                else if ( metaData.type().is_associative_container() )
                {
                    a_Archive << YAML::BeginSeq;
                    for ( auto&& [key, value] : memberData.as_associative_container() )
                    {

                        a_Archive << YAML::Flow << YAML::BeginSeq;
                        if ( auto serializeFunc = key.type().prop( YAMLSerializeFuncID ) )
                        {
                            serializeFunc.value().cast<SerializeFunc>()( a_Archive, key );
                        }
                        if ( auto serializeFunc = value.type().prop( YAMLSerializeFuncID ) )
                        {
                            serializeFunc.value().cast<SerializeFunc>()( a_Archive, value );
                        }
                        a_Archive << YAML::EndSeq;
                    }
                    a_Archive << YAML::EndSeq;
                }
                // Print an error message if there is no serialize function for the type.
                else
                {
                    a_Archive << std::string( "Refl Error: No serialize function for type <" ) + metaData.type().info().name().data(), +">!";
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
    bool DrawBasicType( const char* a_Name, MetaAny& a_Handle, PropertyFlags a_Flags )
    {
        ::Tridium::Editor::EDrawPropertyFlags drawFlags 
            = HasFlag( a_Flags, ::Tridium::Refl::EPropertyFlag::EditAnywhere ) 
            ? ::Tridium::Editor::EDrawPropertyFlags::Editable : ::Tridium::Editor::EDrawPropertyFlags::ReadOnly;

		MetaAny handle = a_Handle.type().is_pointer_like() ? *a_Handle : a_Handle;
        T value = handle.cast<T>();

        if ( ::Tridium::Editor::DrawProperty( a_Name, value, drawFlags ) )
        {
            a_Handle = value;
            return true;
        }

		return false;
    }

    // Adds a static draw property lambda to the type
// Converts the PropertyFlags to DrawPropertyFlags. EditAnywhere -> Editable, VisibleAnywhere -> ReadOnly
// Calls the templated DrawProperty function with the value and flags
#define ADD_DRAWPROPERTY_FUNC_TO_TYPE(Type)                                                                            \
            .prop( ::Tridium::Editor::Internal::DrawPropFuncID,                                                        \
                +[]( const char* a_Name, ::Tridium::Refl::MetaAny& a_Handle, ::Tridium::Refl::PropertyFlags a_Flags )  \
                    -> bool 																						   \
                {                                                                                                      \
                    return ::Tridium::Refl::DrawBasicType<Type>(a_Name, a_Handle, a_Flags);                            \
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
            .prop(Internal::CleanClassNamePropID, #Type) \
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