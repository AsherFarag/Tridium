#include "tripch.h"
#include "Reflection.h"

namespace Tridium::Reflection {

	MetaRegistry& MetaRegistry::Get()
	{
		static MetaRegistry s_Instance;
		return s_Instance;
	}

	class BasicTypes
	{
    public:
		static void OnReflect()
		{
        #define SERIALIZE_BASIC_TYPE(Type) \
            entt::meta<Type>() \
            .type(entt::type_hash<Type>::value()) \
            .prop(_Internal_YAML_OnSerialize_ID, \
                +[](YAML::Emitter& a_Out, const char* a_Name, const entt::meta_handle& a_Value) \
                { \
                    a_Out << YAML::Key << a_Name << YAML::Value << *static_cast<const Type*>(a_Value->data()); \
                });

            SERIALIZE_BASIC_TYPE( bool );
            SERIALIZE_BASIC_TYPE( int );
            SERIALIZE_BASIC_TYPE( int8_t );
            SERIALIZE_BASIC_TYPE( uint8_t );
            SERIALIZE_BASIC_TYPE( int16_t );
            SERIALIZE_BASIC_TYPE( uint16_t );
            SERIALIZE_BASIC_TYPE( int32_t );
            SERIALIZE_BASIC_TYPE( uint32_t );
            SERIALIZE_BASIC_TYPE( int64_t );
            SERIALIZE_BASIC_TYPE( uint64_t );
            SERIALIZE_BASIC_TYPE( float );
            SERIALIZE_BASIC_TYPE( double );
            SERIALIZE_BASIC_TYPE( char );
            SERIALIZE_BASIC_TYPE( unsigned char );
            SERIALIZE_BASIC_TYPE( short );
            SERIALIZE_BASIC_TYPE( unsigned short );
            SERIALIZE_BASIC_TYPE( long );
            SERIALIZE_BASIC_TYPE( unsigned long );
            SERIALIZE_BASIC_TYPE( long long );
            SERIALIZE_BASIC_TYPE( unsigned long long );
            //SERIALIZE_BASIC_TYPE( long double );

        #undef SERIALIZE_BASIC_TYPE
		}
	};

	static Reflector<BasicTypes> s_BasicTypesReflector;

} // namespace Tridium::Reflection