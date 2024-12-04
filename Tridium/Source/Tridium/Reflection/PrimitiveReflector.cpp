#include "tripch.h"
#include "PrimitiveReflector.h"
#include "Reflection.h"
#include <Tridium/IO/TextSerializer.h>
#include <Tridium/Core/Types.h>

namespace Tridium::Refl::Internal {

	using enum EClassFlags;
	using enum EPropertyFlags;

#ifdef IS_EDITOR
	template<typename T>
	bool DrawBasicType( const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags )
	{
		::Tridium::Editor::EDrawPropertyFlags drawFlags
			= HasFlag( a_Flags, ::Tridium::Refl::EPropertyFlags::EditAnywhere )
			? ::Tridium::Editor::EDrawPropertyFlags::Editable : ::Tridium::Editor::EDrawPropertyFlags::ReadOnly;

		MetaAny handle = a_Handle.type().is_pointer_like() ? *a_Handle : a_Handle;
		if ( a_Handle.type().is_pointer_like() )
		{
			// Const cast the pointer if it is a const pointer
			if ( a_Handle.allow_cast<const T*>( ) )
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

#define DRAW_PROP( Type ) \
			.prop( Props::DrawPropertyProp::ID, +[]( const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags ) { return DrawBasicType<Type>( a_Name, a_Handle, a_Flags ); } )
#else
#define DRAW_PROP( Type )
#endif // IS_EDITOR

	template <typename T>
	void SerializePrimitive( IO::Archive& a_Archive, const MetaAny& a_Value )
	{
		IO::SerializeToText( a_Archive, a_Value.cast<const T&>() );
	}

	template <typename T>
	bool DeserializePrimitive( const YAML::Node& a_Node, MetaAny& a_Handle )
	{
		return IO::DeserializeFromText( a_Node, a_Handle.cast<T&>() );
	}

	#define REFLECT_PRIM( Type ) CreateMetaFactory<Type>().type( entt::type_hash<Type>::value() ).prop( Props::CleanClassNameProp::ID, #Type )

	#define FLAGS_PRIM( Flags ) \
			.prop( Props::ClassFlagsProp::ID, Flags )

	#define SERIALIZE(Type) \
            .prop(Props::TextSerializeProp::ID, +[](IO::Archive& a_Archive, const MetaAny& a_Data) { SerializePrimitive<Type>(a_Archive, a_Data); }) \
			.prop(Props::TextDeserializeProp::ID, +[](const YAML::Node& a_Node, MetaAny& a_Data) { DeserializePrimitive<Type>(a_Node, a_Data); })

	void ReflectPrimitiveTypes()
	{
		REFLECT_PRIM( bool )SERIALIZE( bool )DRAW_PROP( bool );
		REFLECT_PRIM( int )SERIALIZE( int )DRAW_PROP( int );
		REFLECT_PRIM( int8_t )SERIALIZE( int8_t )DRAW_PROP( int8_t );
		REFLECT_PRIM( int16_t )SERIALIZE( int16_t )DRAW_PROP( int16_t );
		REFLECT_PRIM( int32_t )SERIALIZE( int32_t )DRAW_PROP( int32_t );
		REFLECT_PRIM( int64_t )SERIALIZE( int64_t )DRAW_PROP( int64_t );
		REFLECT_PRIM( uint8_t )SERIALIZE( uint8_t )DRAW_PROP( uint8_t );
		REFLECT_PRIM( uint16_t )SERIALIZE( uint16_t )DRAW_PROP( uint16_t );
		REFLECT_PRIM( uint32_t )SERIALIZE( uint32_t )DRAW_PROP( uint32_t );
		REFLECT_PRIM( uint64_t )SERIALIZE( uint64_t )DRAW_PROP( uint64_t );
		REFLECT_PRIM( float )SERIALIZE( float )DRAW_PROP( float );
		REFLECT_PRIM( double )SERIALIZE( double )DRAW_PROP( double );

		// STD
		REFLECT_PRIM( std::string )SERIALIZE( std::string )DRAW_PROP( std::string );

		// MATH
		REFLECT_PRIM( Vector2 )FLAGS_PRIM( Scriptable )SERIALIZE( Vector2 )DRAW_PROP( Vector2 );
		REFLECT_PRIM( Vector3 )FLAGS_PRIM( Scriptable )SERIALIZE( Vector3 )DRAW_PROP( Vector3 );
		REFLECT_PRIM( Vector4 )FLAGS_PRIM( Scriptable )SERIALIZE( Vector4 )DRAW_PROP( Vector4 );
		REFLECT_PRIM( iVector2 )FLAGS_PRIM( Scriptable )SERIALIZE( iVector2 )DRAW_PROP( iVector2 );
		REFLECT_PRIM( iVector3 )FLAGS_PRIM( Scriptable )SERIALIZE( iVector3 )DRAW_PROP( iVector3 );
		REFLECT_PRIM( iVector4 )FLAGS_PRIM( Scriptable )SERIALIZE( iVector4 )DRAW_PROP( iVector4 );
		REFLECT_PRIM( Color )FLAGS_PRIM( Scriptable )SERIALIZE( Color )DRAW_PROP( Color );
		REFLECT_PRIM( Rotator )FLAGS_PRIM( Scriptable )SERIALIZE( Rotator )DRAW_PROP( Rotator );

		// TRIDIUM
		REFLECT_PRIM( GUID )FLAGS_PRIM( Scriptable )SERIALIZE( GUID )DRAW_PROP( GUID );
		REFLECT_PRIM( GameObject )FLAGS_PRIM( Scriptable )SERIALIZE( GameObject )DRAW_PROP( GameObject );
		REFLECT_PRIM( AssetHandle )SERIALIZE( AssetHandle )DRAW_PROP( AssetHandle );
		REFLECT_PRIM( SceneHandle )SERIALIZE( SceneHandle )DRAW_PROP( SceneHandle );
		REFLECT_PRIM( MaterialHandle )SERIALIZE( MaterialHandle )DRAW_PROP( MaterialHandle );
		REFLECT_PRIM( TextureHandle )SERIALIZE( TextureHandle )DRAW_PROP( TextureHandle );
		REFLECT_PRIM( MeshSourceHandle )SERIALIZE( MeshSourceHandle )DRAW_PROP( MeshSourceHandle );
		REFLECT_PRIM( StaticMeshHandle )SERIALIZE( StaticMeshHandle )DRAW_PROP( StaticMeshHandle );
		REFLECT_PRIM( ShaderHandle )SERIALIZE( ShaderHandle )DRAW_PROP( ShaderHandle );
		REFLECT_PRIM( CubeMapHandle )SERIALIZE( CubeMapHandle )DRAW_PROP( CubeMapHandle );
		REFLECT_PRIM( LuaHandle )SERIALIZE( LuaHandle )DRAW_PROP( LuaHandle );

		// TEMP?
		REFLECT_PRIM( SharedPtr<Framebuffer> )DRAW_PROP( SharedPtr<Framebuffer> );
	}
}
