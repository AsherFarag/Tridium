#include "tripch.h"
#include "Scriptables.h"
#include "ScriptEngine.h"

#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types.h>

#include <Tridium/Math/Math.h>
#include <Tridium/Math/Rotator.h>

namespace Tridium::Script {

	//////////////////////////////////////////////////////////////////////////
	// GameObject
	//////////////////////////////////////////////////////////////////////////

	void Scriptable<GameObject>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<GameObject>( "GameObject" );
		type["GetTransform"] = &GameObject::GetTransform;
		type["GetName"] = []( GameObject& a_GameObject ) -> std::string { return a_GameObject.GetTag(); };
		type["Destroy"] = &GameObject::Destroy;
	}

	//////////////////////////////////////////////////////////////////////////
	// Math
	//////////////////////////////////////////////////////////////////////////

	template <typename _Vector>
	void RegisterVectorFunctions( sol::usertype<_Vector>& a_Type )
	{
		a_Type["Normalized"] = glm::normalize<_Vector::length(), float, glm::packed_highp>;
		a_Type["Length"] = glm::length<_Vector::length(), float, glm::packed_highp>;
		a_Type["Dot"] = glm::dot<_Vector::length(), float, glm::packed_highp>;
	}

	void Scriptable<Vector2>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Vector2>( "Vector2" );
		type["new"] = sol::constructors<Vector2(), Vector2( float ), Vector2( float, float )>();
		type["x"] = sol::property( &Vector2::x, &Vector2::x );
		type["y"] = sol::property( &Vector2::y, &Vector2::y );
		RegisterVectorFunctions( type );
	}

	void Scriptable<Vector3>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Vector3>( "Vector3" );
		type["new"] = sol::constructors<Vector3(), Vector3( float ), Vector3( float, float, float )>();
		type["x"] = sol::property( &Vector3::x, &Vector3::x );
		type["y"] = sol::property( &Vector3::y, &Vector3::y );
		type["z"] = sol::property( &Vector3::z, &Vector3::z );
		RegisterVectorFunctions( type );
	}

	void Scriptable<Vector4>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Vector4>( "Vector4" );
		type["new"] = sol::constructors<Vector4(), Vector4( float ), Vector4( float, float, float, float )>();
		type["x"] = sol::property( &Vector4::x, &Vector4::x );
		type["y"] = sol::property( &Vector4::y, &Vector4::y );
		type["z"] = sol::property( &Vector4::z, &Vector4::z );
		type["w"] = sol::property( &Vector4::w, &Vector4::w );
		RegisterVectorFunctions( type );
	}

	void Scriptable<iVector2>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<iVector2>( "iVector2" );
		type["new"] = sol::constructors<iVector2(), iVector2( int ), iVector2( int, int )>();
		type["x"] = sol::property( &iVector2::x, &iVector2::x );
		type["y"] = sol::property( &iVector2::y, &iVector2::y );
		RegisterVectorFunctions( type );
	}

	void Scriptable<iVector3>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<iVector3>( "iVector3" );
		type["new"] = sol::constructors<iVector3(), iVector3( int ), iVector3( int, int, int )>();
		type["x"] = sol::property( &iVector3::x, &iVector3::x );
		type["y"] = sol::property( &iVector3::y, &iVector3::y );
		type["z"] = sol::property( &iVector3::z, &iVector3::z );
		RegisterVectorFunctions( type );
	}

	void Scriptable<iVector4>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<iVector4>( "iVector4" );
		type["new"] = sol::constructors<iVector4(), iVector4( int ), iVector4( int, int, int, int )>();
		type["x"] = sol::property( &iVector4::x, &iVector4::x );
		type["y"] = sol::property( &iVector4::y, &iVector4::y );
		type["z"] = sol::property( &iVector4::z, &iVector4::z );
		type["w"] = sol::property( &iVector4::w, &iVector4::w );
		RegisterVectorFunctions( type );
	}

	template <typename _Matrix>
	void RegisterMatrixFunctions( sol::usertype<_Matrix>& a_Type )
	{
		a_Type["Identity"] = +[]() -> _Matrix { return _Matrix( 1.0f ); };
	}

	void Scriptable<Matrix2>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Matrix2>( "Matrix2" );
		type["new"] = sol::constructors<Matrix2(), Matrix2( float ), Matrix2( const Matrix2& )>();
		RegisterMatrixFunctions( type );
	}

	void Scriptable<Matrix3>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Matrix3>( "Matrix3" );
		type["new"] = sol::constructors<Matrix3(), Matrix3( float ), Matrix3( const Matrix3& )>();
		RegisterMatrixFunctions( type );
	}

	void Scriptable<Matrix4>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Matrix4>( "Matrix4" );
		type["new"] = sol::constructors<Matrix4(), Matrix4( float ), Matrix4( const Matrix4& )>();
		RegisterMatrixFunctions( type );
	}

	void Scriptable<Rotator>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Rotator>( "Rotator" );
		type["new"] = sol::constructors<Rotator()>();

		type["Pitch"] = sol::property( 
			+[](const Rotator& a_Rotator) -> float { return glm::degrees(a_Rotator.Euler.x); },
			+[]( Rotator& a_Rotator, float a_Value ) { a_Rotator.Euler.x = glm::radians( a_Value ); } );

		type["Yaw"] = sol::property(
			+[]( const Rotator& a_Rotator ) -> float { return glm::degrees( a_Rotator.Euler.y ); },
			+[]( Rotator& a_Rotator, float a_Value ) { a_Rotator.Euler.y = glm::radians( a_Value ); } );

		type["Roll"] = sol::property(
			+[]( const Rotator& a_Rotator ) -> float { return glm::degrees( a_Rotator.Euler.z ); },
			+[]( Rotator& a_Rotator, float a_Value ) { a_Rotator.Euler.z = glm::radians( a_Value ); } );

		type["Forward"] = sol::property(
			+[]( const Rotator& a_Rotator ) -> Vector3 { return a_Rotator.GetForward(); },
			+[]( Rotator& a_Rotator, const Vector3& a_Value ) { a_Rotator.SetFromQuaternion( glm::quatLookAt( a_Value, Vector3( 0.0f, 1.0f, 0.0f ) ) ); } );

		type["Right"] = sol::property(
			+[]( const Rotator& a_Rotator ) -> Vector3 { return a_Rotator.GetRight(); },
			+[]( Rotator& a_Rotator, const Vector3& a_Value ) { a_Rotator.SetFromQuaternion( glm::quatLookAt( Vector3( 0.0f, 1.0f, 0.0f ), a_Value ) ); } );

		type["Up"] = sol::property(
			+[]( const Rotator& a_Rotator ) -> Vector3 { return a_Rotator.GetUp(); },
			+[]( Rotator& a_Rotator, const Vector3& a_Value ) { a_Rotator.SetFromQuaternion( glm::quatLookAt( Vector3( 0.0f, 0.0f, 1.0f ), a_Value ) ); } );
	}

	void Scriptable<Color>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Color>( "Color" );
		type["new"] = sol::constructors<Color(), Color( float ), Color( float, float, float, float )>();
		type["r"] = sol::property( &Color::r, &Color::r );
		type["g"] = sol::property( &Color::g, &Color::g );
		type["b"] = sol::property( &Color::b, &Color::b );
		type["a"] = sol::property( &Color::a, &Color::a );
	}

	//////////////////////////////////////////////////////////////////////////
}