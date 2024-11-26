#include "tripch.h"
#include "Reflection.h"
#include "EditorReflection.h"
#include <Editor/PropertyDrawers.h>
#include <Tridium/IO/TextSerializer.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Material.h>
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Math/Rotator.h>
#include <Tridium/ECS/GameObject.h>
#include <Tridium/Rendering/FrameBuffer.h>


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

				// Ensure memberData is a value reference and not a pointer
				MetaAny memberData = a_Data.type().is_pointer_like() ? 
                    metaData.get( *a_Data ) 
                    : metaData.get( a_Data );

				if ( memberData.type().is_pointer_like() )
				{
					memberData = *memberData;
				}

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

        void DeserializeMembersOfMetaClass( const YAML::Node& a_Node, MetaAny& a_Data, const MetaType& a_MetaType )
        {
            for ( auto&& [id, metaData] : a_MetaType.data() )
            {
                if ( !HasFlag( metaData.propFlags(), EPropertyFlag::Serialize ) )
                {
                    continue;
                }

                auto propNode = a_Node[metaData.name().c_str()];
                if ( !propNode )
                    continue;

                MetaAny memberData = a_Data.type().is_pointer_like() ? metaData.get( *a_Data ) : metaData.get( a_Data );
				if ( memberData.type().is_pointer_like() )
				{
					memberData = *memberData;
				}

				if (auto deserializePropFunc = metaData.type().prop(YAMLDeserializeFuncID) )
                {
                    deserializePropFunc.value().cast<DeserializeFunc>()( propNode, memberData );
                }
                else if ( metaData.type().is_sequence_container() && propNode.IsSequence() )
                {
					auto seqContainer = memberData.as_sequence_container();
                    if ( auto deserializePropFunc = seqContainer.value_type().prop(YAMLDeserializeFuncID) )
                    {
						seqContainer.clear();
                        auto deserializeFunc = deserializePropFunc.value().cast<DeserializeFunc>();
                        for ( auto elem : propNode )
                        {
                            auto metaElem = seqContainer.value_type().construct();
							deserializeFunc( elem, metaElem );
                            seqContainer.insert( seqContainer.end(), metaElem );
                        }
                    }
				}
				else if ( metaData.type().is_associative_container() )
				{
					auto assocContainer = memberData.as_associative_container();
                    if ( auto deserializeKeyFunc = assocContainer.key_type().prop( YAMLDeserializeFuncID ) )
                    {
                        if ( auto deserializeValueFunc = assocContainer.mapped_type().prop( YAMLDeserializeFuncID ) )
                        {
                            assocContainer.clear();
                            auto deserializeKey = deserializeKeyFunc.value().cast<DeserializeFunc>();
                            auto deserializeValue = deserializeValueFunc.value().cast<DeserializeFunc>();
                            for ( auto elem : propNode )
                            {
                                auto key = assocContainer.key_type().construct();
                                auto value = assocContainer.mapped_type().construct();
                                deserializeKey( elem[0], key );
                                deserializeValue( elem[1], value );
                                assocContainer.insert( key, value );
                            }
                        }
                    }
				}       
            }

        }

        void DeserializeClass( const YAML::Node& a_Node, MetaAny& a_Data, const MetaType& a_MetaType )
        {
            for ( auto&& [id, baseType] : a_MetaType.base() )
            {
                DeserializeMembersOfMetaClass( a_Node, a_Data, baseType );
            }

            DeserializeMembersOfMetaClass( a_Node, a_Data, a_MetaType );
        }
    }

#if IS_EDITOR

    template<typename T>
    bool DrawBasicType( const char* a_Name, MetaAny& a_Handle, PropertyFlags a_Flags )
    {
        ::Tridium::Editor::EDrawPropertyFlags drawFlags 
            = HasFlag( a_Flags, ::Tridium::Refl::EPropertyFlag::EditAnywhere ) 
            ? ::Tridium::Editor::EDrawPropertyFlags::Editable : ::Tridium::Editor::EDrawPropertyFlags::ReadOnly;

		MetaAny handle = a_Handle.type().is_pointer_like() ? *a_Handle : a_Handle;
        if ( a_Handle.type().is_pointer_like() )
        {
			// Const cast the pointer if it is a const pointer
			if ( a_Handle.allow_cast<const T*>() )
                return ::Tridium::Editor::DrawProperty( a_Name, *const_cast<T*>( a_Handle.cast<const T*>() ), drawFlags );
            else
				return ::Tridium::Editor::DrawProperty( a_Name, *a_Handle.cast<T*>(), drawFlags );
        }
        else
        {
            if ( a_Handle.policy() != entt::any_policy::cref )
            {
                return ::Tridium::Editor::DrawProperty( a_Name, a_Handle.cast<T&>(), drawFlags );
            }
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

    template <typename T>
	void SerializeBasicType( IO::Archive& a_Archive, const MetaAny& a_Value )
	{
        IO::SerializeToText( a_Archive, a_Value.cast<const T&>() );
	}

    template <typename T>
    bool DeserializeBasicType( const YAML::Node& a_Node, MetaAny& a_Handle )
    {
        return IO::DeserializeFromText( a_Node, a_Handle.cast<T&>() );
    }

#define ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE(Type)                                                          \
            .prop(Internal::YAMLSerializeFuncID,                                                          \
                +[]( IO::Archive& a_Archive, const ::Tridium::Refl::MetaAny& a_Value )                    \
                {                                                                                         \
                    SerializeBasicType<Type>(a_Archive, a_Value);                                         \
                })																						  \
			.prop(Internal::YAMLDeserializeFuncID,                                                        \
				+[]( const YAML::Node& a_Node, ::Tridium::Refl::MetaAny& a_Value )                        \
				{                                                                                         \
					DeserializeBasicType<Type>(a_Node, a_Value);                                          \
				})

#define REFLECT_BASIC_TYPE(Type)                  \
			entt::meta<Type>()                    \
			.type(entt::type_hash<Type>::value()) \
            .prop(Internal::CleanClassNamePropID, #Type)

    void __Internal_InitializeReflection()
    {
		// Basic types
		REFLECT_BASIC_TYPE( bool )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( bool )ADD_DRAWPROPERTY_FUNC_TO_TYPE( bool );
		REFLECT_BASIC_TYPE( int )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( int ) ADD_DRAWPROPERTY_FUNC_TO_TYPE( int );
        REFLECT_BASIC_TYPE( int8_t )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( int8_t )ADD_DRAWPROPERTY_FUNC_TO_TYPE( int8_t );
		REFLECT_BASIC_TYPE( uint8_t )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( uint8_t )ADD_DRAWPROPERTY_FUNC_TO_TYPE( uint8_t );
        REFLECT_BASIC_TYPE( int16_t )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( int16_t )ADD_DRAWPROPERTY_FUNC_TO_TYPE( int16_t );
        REFLECT_BASIC_TYPE( uint16_t )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( uint16_t )ADD_DRAWPROPERTY_FUNC_TO_TYPE( uint16_t );
		REFLECT_BASIC_TYPE( int32_t )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( int32_t )ADD_DRAWPROPERTY_FUNC_TO_TYPE( int32_t );
        REFLECT_BASIC_TYPE( uint32_t )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( uint32_t )ADD_DRAWPROPERTY_FUNC_TO_TYPE( uint32_t );
        REFLECT_BASIC_TYPE( int64_t )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( int64_t )ADD_DRAWPROPERTY_FUNC_TO_TYPE( int64_t );
        REFLECT_BASIC_TYPE( uint64_t )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( uint64_t )ADD_DRAWPROPERTY_FUNC_TO_TYPE( uint64_t );
        REFLECT_BASIC_TYPE( float )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( float )ADD_DRAWPROPERTY_FUNC_TO_TYPE( float );
        REFLECT_BASIC_TYPE( double )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( double )ADD_DRAWPROPERTY_FUNC_TO_TYPE( double );
        REFLECT_BASIC_TYPE( char );
        REFLECT_BASIC_TYPE( char* );
		REFLECT_BASIC_TYPE( const char* )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( std::string );

        // STD
		REFLECT_BASIC_TYPE( std::string )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( std::string )ADD_DRAWPROPERTY_FUNC_TO_TYPE( std::string );

		// Math
		REFLECT_BASIC_TYPE( Vector2 )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( Vector2 )ADD_DRAWPROPERTY_FUNC_TO_TYPE( Vector2 );
		REFLECT_BASIC_TYPE( Vector3 )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( Vector3 )ADD_DRAWPROPERTY_FUNC_TO_TYPE( Vector3 );
		REFLECT_BASIC_TYPE( Vector4 )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( Vector4 )ADD_DRAWPROPERTY_FUNC_TO_TYPE( Vector4 );
		REFLECT_BASIC_TYPE( iVector2 )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( iVector2 )ADD_DRAWPROPERTY_FUNC_TO_TYPE( iVector2 );
        REFLECT_BASIC_TYPE( iVector3 )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( iVector3 )ADD_DRAWPROPERTY_FUNC_TO_TYPE( iVector3 );
        REFLECT_BASIC_TYPE( iVector4 )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( iVector4 )ADD_DRAWPROPERTY_FUNC_TO_TYPE( iVector4 );
		REFLECT_BASIC_TYPE( Color )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( Color )ADD_DRAWPROPERTY_FUNC_TO_TYPE( Color );
		REFLECT_BASIC_TYPE( Rotator )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( Rotator )ADD_DRAWPROPERTY_FUNC_TO_TYPE( Rotator );

        // Tridium
		REFLECT_BASIC_TYPE( GUID )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( GUID )ADD_DRAWPROPERTY_FUNC_TO_TYPE( GUID );
		REFLECT_BASIC_TYPE( GameObject )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( GameObject )ADD_DRAWPROPERTY_FUNC_TO_TYPE( GameObject );
		REFLECT_BASIC_TYPE( AssetHandle )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( AssetHandle )ADD_DRAWPROPERTY_FUNC_TO_TYPE( AssetHandle );
		REFLECT_BASIC_TYPE( SceneHandle )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( SceneHandle )ADD_DRAWPROPERTY_FUNC_TO_TYPE( SceneHandle );
		REFLECT_BASIC_TYPE( MaterialHandle )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( MaterialHandle )ADD_DRAWPROPERTY_FUNC_TO_TYPE( MaterialHandle );
		REFLECT_BASIC_TYPE( MeshSourceHandle )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( MeshSourceHandle )ADD_DRAWPROPERTY_FUNC_TO_TYPE( MeshSourceHandle );
		REFLECT_BASIC_TYPE( StaticMeshHandle )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( StaticMeshHandle )ADD_DRAWPROPERTY_FUNC_TO_TYPE( StaticMeshHandle );
		REFLECT_BASIC_TYPE( ShaderHandle )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( ShaderHandle )ADD_DRAWPROPERTY_FUNC_TO_TYPE( ShaderHandle );
		REFLECT_BASIC_TYPE( TextureHandle )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( TextureHandle )ADD_DRAWPROPERTY_FUNC_TO_TYPE( TextureHandle );
		REFLECT_BASIC_TYPE( CubeMapHandle )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( CubeMapHandle )ADD_DRAWPROPERTY_FUNC_TO_TYPE( CubeMapHandle );
		REFLECT_BASIC_TYPE( LuaHandle )ADD_SERIALIZE_TO_TEXT_FUNC_TO_TYPE( LuaHandle )ADD_DRAWPROPERTY_FUNC_TO_TYPE( LuaHandle );


        // Temp?
        REFLECT_BASIC_TYPE( SharedPtr<Framebuffer> )ADD_DRAWPROPERTY_FUNC_TO_TYPE( SharedPtr<Framebuffer> );
    }

} // namespace Tridium::Reflection