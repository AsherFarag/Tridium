#include "tripch.h"
#include "ReflectionImpl.h"

namespace Tridium::Refl::Internal {

    void SerializeMembersOfMetaClass( IO::Archive& a_Archive, const MetaType& a_MetaType, const MetaAny& a_Data )
    {
        for ( auto&& [id, metaData] : a_MetaType.Properties() )
        {
            if ( !HasFlag( metaData.propFlags(), EPropertyFlags::Serialize ) )
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

			MetaType propType = memberData.type();

            // If the type has a serialize function, call it.
            if ( auto serializeFunc = propType.TryGetTextSerializeFunc() )
            {
                serializeFunc( a_Archive, memberData );
            }


			// If the type is a sequence container, serialize each element.
            else if ( propType.IsSequenceContainer() )
            {
                a_Archive << YAML::Flow << YAML::BeginSeq;
                auto seqContainer = memberData.as_sequence_container();
				MetaType elemType = seqContainer.value_type();

                if ( auto serializeFunc = elemType.TryGetTextSerializeFunc() )
                {
                    for ( auto&& element : seqContainer )
                    {
						serializeFunc( a_Archive, element );
                    }
                }
                a_Archive << YAML::EndSeq;
            }


            // If the type is an associative container, serialize each key-value pair.
            else if ( metaData.type().is_associative_container() )
            {
                a_Archive << YAML::BeginSeq;
				auto assocContainer = memberData.as_associative_container();
				MetaType keyType = assocContainer.key_type();
				MetaType valueType = assocContainer.mapped_type();

				TextSerializeFunc serializeKeyFunc = keyType.TryGetTextSerializeFunc();
				TextSerializeFunc serializeValueFunc = valueType.TryGetTextSerializeFunc();

				if ( serializeKeyFunc && serializeValueFunc )
                {
                    for ( auto&& [key, value] : assocContainer  )
                    {
                        a_Archive << YAML::Flow << YAML::BeginSeq;
						serializeKeyFunc( a_Archive, key );
						serializeValueFunc( a_Archive, value );
                        a_Archive << YAML::EndSeq;
                    }
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

        for ( auto&& [id, baseType] : a_MetaType.Bases() )
        {
            SerializeMembersOfMetaClass( a_Archive, baseType, a_Data );
        }

        SerializeMembersOfMetaClass( a_Archive, a_MetaType, a_Data );

        a_Archive << YAML::EndMap;
    }

    void DeserializeMembersOfMetaClass( const YAML::Node& a_Node, MetaAny& a_Data, const MetaType& a_MetaType )
    {
        for ( auto&& [id, metaData] : a_MetaType.Properties() )
        {
            if ( !HasFlag( metaData.propFlags(), EPropertyFlags::Serialize ) )
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

			MetaType memberType = memberData.type();

			// If the type has a deserialize function, call it.
            if ( auto deserializePropFunc = memberType.TryGetTextDeserializeFunc() )
            {
                deserializePropFunc( propNode, memberData );
            }


			// If the type is a sequence container, deserialize each element.
            else if ( metaData.type().is_sequence_container() && propNode.IsSequence() )
            {
                auto seqContainer = memberData.as_sequence_container();
				MetaType elemType = seqContainer.value_type();
                if ( auto deserializeFunc = elemType.TryGetTextDeserializeFunc() )
                {
                    seqContainer.clear();
                    for ( auto elem : propNode )
                    {
                        auto metaElem = elemType.Construct();
                        deserializeFunc( elem, metaElem );
                        seqContainer.insert( seqContainer.end(), metaElem );
                    }
                }
            }


			// If the type is an associative container, deserialize each key-value pair.
            else if ( metaData.type().is_associative_container() )
            {
                auto assocContainer = memberData.as_associative_container();
				MetaType keyType = assocContainer.key_type();
				MetaType valueType = assocContainer.mapped_type();

				auto deserializeKeyFunc = keyType.TryGetTextDeserializeFunc();
				auto deserializeValueFunc = valueType.TryGetTextDeserializeFunc();

				if ( deserializeKeyFunc && deserializeValueFunc )
				{
                    assocContainer.clear();
                    for ( auto elem : propNode )
                    {
                        auto key = keyType.Construct();
                        auto value = valueType.Construct();
                        deserializeKeyFunc( elem[0], key );
                        deserializeValueFunc( elem[1], value );
                        assocContainer.insert( key, value );
                    }
				}
            }
        }

    }

    void DeserializeClass( const YAML::Node& a_Node, MetaAny& a_Data, const MetaType& a_MetaType )
    {
        for ( auto&& [id, baseType] : a_MetaType.Bases() )
        {
            DeserializeMembersOfMetaClass( a_Node, a_Data, baseType );
        }

        DeserializeMembersOfMetaClass( a_Node, a_Data, a_MetaType );
    }

}