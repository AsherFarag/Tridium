#include "tripch.h"
#include "PrimitiveReflector.h"
#include "Reflection.h"
#include <Tridium/IO/TextSerializer.h>
#include <Tridium/Core/Types.h>
#include <Tridium/Scripting/Scriptables.h>
#include <Tridium/Math/Math.h>
#include <Tridium/Math/Rotator.h>

namespace Tridium::Refl::Internal {

	using namespace entt::literals;

	#pragma region Helpers

#ifdef IS_EDITOR
	template<typename T>
	bool DrawBasicType( const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags )
	{
		::Tridium::EDrawPropertyFlags drawFlags
			= EnumFlags( a_Flags ).HasFlag( ::Tridium::Refl::EPropertyFlags::EditAnywhere )
			? ::Tridium::EDrawPropertyFlags::Editable : ::Tridium::EDrawPropertyFlags::ReadOnly;

		MetaAny handle = a_Handle.type().is_pointer_like() ? *a_Handle : a_Handle;
		if ( a_Handle.type().is_pointer_like() )
		{
			// Const cast the pointer if it is a const pointer
			if ( a_Handle.allow_cast<const T*>( ) )
				return ::Tridium::Editor::DrawProperty( a_Name, *ConstCast<T*>( a_Handle.cast<const T*>() ), drawFlags );
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
			factory.prop( Props::DrawPropertyProp::ID, +[]( const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags ) { return DrawBasicType<Type>( a_Name, a_Handle, a_Flags ); } );

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

	#define END_PRIM ; }

	#define REFLECT_PRIM( Type ) \
		 { \
			MetaFactory<Type> factory = CreateMetaFactory<Type>(); \
	        factory.type( entt::type_hash<Type>::value() ); \
			factory.prop( Props::CleanClassNameProp::ID, #Type ); \
        

	#define FLAGS_PRIM( Flags ) \
			factory.prop( Props::ClassFlagsProp::ID, Flags );

	#define SERIALIZE(Type) \
            factory.prop(Props::TextSerializeProp::ID, +[](IO::Archive& a_Archive, const MetaAny& a_Data) { SerializePrimitive<Type>(a_Archive, a_Data); }); \
			factory.prop(Props::TextDeserializeProp::ID, +[](const YAML::Node& a_Node, MetaAny& a_Data) { DeserializePrimitive<Type>(a_Node, a_Data); });

	template <typename _Vector>
	MetaFactory<_Vector> ReflectVectorType( const char* a_Name )
	{
		MetaFactory<_Vector> factory = CreateMetaFactory<_Vector>();
		factory.type( entt::type_hash<_Vector>::value() );
		factory.prop( Props::CleanClassNameProp::ID, a_Name );
		factory.prop( Props::ClassFlagsProp::ID, EClassFlags::Scriptable );
		factory.prop( Props::TextSerializeProp::ID, +[]( IO::Archive& a_Archive, const MetaAny& a_Data ) { SerializePrimitive<_Vector>( a_Archive, a_Data ); } );
		factory.prop( Props::TextDeserializeProp::ID, +[]( const YAML::Node& a_Node, MetaAny& a_Data ) { DeserializePrimitive<_Vector>( a_Node, a_Data ); } );
		factory.prop( Props::DrawPropertyProp::ID, +[]( const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags ) { return DrawBasicType<_Vector>( a_Name, a_Handle, a_Flags ); } );
		factory.prop( Props::RegisterScriptableProp::ID, +[]( ScriptEngine& a_ScriptEngine ) { Scriptable<_Vector>::RegisterType( a_ScriptEngine ); } );
		return factory;
	}

#pragma endregion

	void ReflectPrimitiveTypes()
	{
		REFLECT_PRIM( bool )SERIALIZE( bool )DRAW_PROP( bool )END_PRIM;
		REFLECT_PRIM( int )SERIALIZE( int )DRAW_PROP( int )END_PRIM;
		REFLECT_PRIM( int8_t )SERIALIZE( int8_t )DRAW_PROP( int8_t )END_PRIM;
		REFLECT_PRIM( int16_t )SERIALIZE( int16_t )DRAW_PROP( int16_t )END_PRIM;
		REFLECT_PRIM( int32_t )SERIALIZE( int32_t )DRAW_PROP( int32_t )END_PRIM;
		REFLECT_PRIM( int64_t )SERIALIZE( int64_t )DRAW_PROP( int64_t )END_PRIM;
		REFLECT_PRIM( uint8_t )SERIALIZE( uint8_t )DRAW_PROP( uint8_t )END_PRIM;
		REFLECT_PRIM( uint16_t )SERIALIZE( uint16_t )DRAW_PROP( uint16_t )END_PRIM;
		REFLECT_PRIM( uint32_t )SERIALIZE( uint32_t )DRAW_PROP( uint32_t )END_PRIM;
		REFLECT_PRIM( uint64_t )SERIALIZE( uint64_t )DRAW_PROP( uint64_t )END_PRIM;
		REFLECT_PRIM( float )SERIALIZE( float )DRAW_PROP( float )END_PRIM;
		REFLECT_PRIM( double )SERIALIZE( double )DRAW_PROP( double )END_PRIM;

		// STD
		REFLECT_PRIM( std::string )SERIALIZE( std::string )DRAW_PROP( std::string )END_PRIM;

		// MATH
		{
			// Vector2
			{
				ReflectVectorType<Vector2>( "Vector2" )
					.data<&Vector2::x>( "x"_hs, EPropertyFlags::ScriptReadWrite, "x", {} )
					.data<&Vector2::y>( "y"_hs, EPropertyFlags::ScriptReadWrite, "y", {} );
			}

			// Vector3
			{
				ReflectVectorType<Vector3>( "Vector3" )
					.data<&Vector3::x>( "x"_hs, EPropertyFlags::ScriptReadWrite, "x", {} )
					.data<&Vector3::y>( "y"_hs, EPropertyFlags::ScriptReadWrite, "y", {} )
					.data<&Vector3::z>( "z"_hs, EPropertyFlags::ScriptReadWrite, "z", {} );
			}

			// Vector4
			{
				ReflectVectorType<Vector4>( "Vector4" )
					.data<&Vector4::x>( "x"_hs, EPropertyFlags::ScriptReadWrite, "x", {} )
					.data<&Vector4::y>( "y"_hs, EPropertyFlags::ScriptReadWrite, "y", {} )
					.data<&Vector4::z>( "z"_hs, EPropertyFlags::ScriptReadWrite, "z", {} )
					.data<&Vector4::w>( "w"_hs, EPropertyFlags::ScriptReadWrite, "w", {} );
			}

			// iVector2
			{
				ReflectVectorType<iVector2>( "iVector2" )
					.data<&iVector2::x>( "x"_hs, EPropertyFlags::ScriptReadWrite, "x", {} )
					.data<&iVector2::y>( "y"_hs, EPropertyFlags::ScriptReadWrite, "y", {} );
			}

			// iVector3
			{
				ReflectVectorType<iVector3>( "iVector3" )
					.data<&iVector3::x>( "x"_hs, EPropertyFlags::ScriptReadWrite, "x", {} )
					.data<&iVector3::y>( "y"_hs, EPropertyFlags::ScriptReadWrite, "y", {} )
					.data<&iVector3::z>( "z"_hs, EPropertyFlags::ScriptReadWrite, "z", {} );
			}

			// iVector4
			{
				ReflectVectorType<iVector4>( "iVector4" )
					.data<&iVector4::x>( "x"_hs, EPropertyFlags::ScriptReadWrite, "x", {} )
					.data<&iVector4::y>( "y"_hs, EPropertyFlags::ScriptReadWrite, "y", {} )
					.data<&iVector4::z>( "z"_hs, EPropertyFlags::ScriptReadWrite, "z", {} )
					.data<&iVector4::w>( "w"_hs, EPropertyFlags::ScriptReadWrite, "w", {} );
			}

			// Color
			{
				ReflectVectorType<Color>( "Color" )
					.data<&Color::r>( "r"_hs, EPropertyFlags::ScriptReadWrite, "r", {} )
					.data<&Color::g>( "g"_hs, EPropertyFlags::ScriptReadWrite, "g", {} )
					.data<&Color::b>( "b"_hs, EPropertyFlags::ScriptReadWrite, "b", {} )
					.data<&Color::a>( "a"_hs, EPropertyFlags::ScriptReadWrite, "a", {} );
			}

			// Rotator 
			{
				Refl::CreateMetaFactory<Rotator>()
					.type( entt::type_hash<Rotator>::value() )
					.prop( Props::CleanClassNameProp::ID, "Rotator" )
					.prop( Props::ClassFlagsProp::ID, EClassFlags::Scriptable )
					.prop( Props::TextSerializeProp::ID, +[]( IO::Archive& a_Archive, const MetaAny& a_Data ) { SerializePrimitive<Rotator>( a_Archive, a_Data ); } )
					.prop( Props::TextDeserializeProp::ID, +[]( const YAML::Node& a_Node, MetaAny& a_Data ) { DeserializePrimitive<Rotator>( a_Node, a_Data ); } )
					.prop( Props::DrawPropertyProp::ID, +[]( const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags ) { return DrawBasicType<Rotator>( a_Name, a_Handle, a_Flags ); } )
					.prop( Props::RegisterScriptableProp::ID, +[]( ScriptEngine& a_ScriptEngine ) { Scriptable<Rotator>::RegisterType( a_ScriptEngine ); } )
					.data<&Rotator::Euler>( "Euler"_hs, EPropertyFlags::ScriptReadWrite, "Euler", {} )
					.data<&Rotator::Quat>( "Quat"_hs, EPropertyFlags::ScriptReadWrite, "Quat", {} );
			}

			// TRIDIUM

			// GameObject
			{
				Refl::CreateMetaFactory<GameObject>()
					.type( entt::type_hash<GameObject>::value() )
					.prop( Props::CleanClassNameProp::ID, "GameObject" )
					.prop( Props::ClassFlagsProp::ID, EClassFlags::Scriptable )
					.prop( Props::TextSerializeProp::ID, +[]( IO::Archive& a_Archive, const MetaAny& a_Data ) { SerializePrimitive<GameObject>( a_Archive, a_Data ); } )
					.prop( Props::TextDeserializeProp::ID, +[]( const YAML::Node& a_Node, MetaAny& a_Data ) { DeserializePrimitive<GameObject>( a_Node, a_Data ); } )
					.prop( Props::DrawPropertyProp::ID, +[]( const char* a_Name, MetaAny& a_Handle, EPropertyFlags a_Flags ) { return DrawBasicType<GameObject>( a_Name, a_Handle, a_Flags ); } )
					.prop( Props::RegisterScriptableProp::ID, +[]( ScriptEngine& a_ScriptEngine ) { Scriptable<GameObject>::RegisterType( a_ScriptEngine ); } );
			}

			REFLECT_PRIM( GUID )FLAGS_PRIM( EClassFlags::Scriptable )SERIALIZE( GUID )DRAW_PROP( GUID )END_PRIM;
			REFLECT_PRIM( AssetHandle )SERIALIZE( AssetHandle )DRAW_PROP( AssetHandle )END_PRIM;
			REFLECT_PRIM( SceneHandle )SERIALIZE( SceneHandle )DRAW_PROP( SceneHandle )END_PRIM;
			REFLECT_PRIM( MaterialHandle )SERIALIZE( MaterialHandle )DRAW_PROP( MaterialHandle )END_PRIM;
			REFLECT_PRIM( TextureHandle )SERIALIZE( TextureHandle )DRAW_PROP( TextureHandle )END_PRIM;
			REFLECT_PRIM( MeshSourceHandle )SERIALIZE( MeshSourceHandle )DRAW_PROP( MeshSourceHandle )END_PRIM;
			REFLECT_PRIM( StaticMeshHandle )SERIALIZE( StaticMeshHandle )DRAW_PROP( StaticMeshHandle )END_PRIM;
			REFLECT_PRIM( ShaderHandle )SERIALIZE( ShaderHandle )DRAW_PROP( ShaderHandle )END_PRIM;
			REFLECT_PRIM( CubeMapHandle )SERIALIZE( CubeMapHandle )DRAW_PROP( CubeMapHandle )END_PRIM;
			REFLECT_PRIM( LuaScriptHandle )SERIALIZE( LuaScriptHandle )DRAW_PROP( LuaScriptHandle )END_PRIM;

			// TEMP?
			REFLECT_PRIM( SharedPtr<Framebuffer> )DRAW_PROP( SharedPtr<Framebuffer> )END_PRIM;
		}

	}
} // namespace Tridium::Refl::Internal
