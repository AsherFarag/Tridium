#include "tripch.h"
#include "Scriptables.h"
#include "ScriptEngine.h"

#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types.h>

#include <Tridium/Math/Math.h>
#include <Tridium/Math/Rotator.h>

namespace Tridium {

	//////////////////////////////////////////////////////////////////////////
	// GameObject
	//////////////////////////////////////////////////////////////////////////

	void Scriptable<OldGameObject>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<OldGameObject>( "GameObject" );
		type["GetTransform"] = &OldGameObject::GetTransform;
		type["GetName"] = []( OldGameObject& a_GameObject ) -> std::string { return a_GameObject.GetTag(); };
		type["Destroy"] = &OldGameObject::Destroy;
	}

	//////////////////////////////////////////////////////////////////////////
	// Math
	//////////////////////////////////////////////////////////////////////////

	template <typename _Vector>
	void RegisterVectorFunctions( sol::usertype<_Vector>& a_Type )
	{
		a_Type["Zero"] = +[]() -> _Vector { return _Vector( 0.0f ); };

		a_Type["Normalized"] = +[]( const _Vector& a_Vector ) -> _Vector { return a_Vector.Normalized(); };
		a_Type["Length"] = +[]( const _Vector& a_Vector ) -> float { return a_Vector.Length(); };
		a_Type["Dot"] = +[]( const _Vector& a_A, const _Vector& a_B ) -> float { return Math::Dot( a_A, a_B ); };
	}

	void Scriptable<Vector2>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Vector2>( "Vector2" );
		type["new"] = sol::constructors<Vector2(), Vector2( float ), Vector2( float, float )>();
		type["X"] = sol::property( &Vector2::X, &Vector2::X );
		type["Y"] = sol::property( &Vector2::Y, &Vector2::Y );
		RegisterVectorFunctions( type );
	}

	void Scriptable<Vector3>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Vector3>( "Vector3" );
		type["new"] = sol::constructors<Vector3(), Vector3( float ), Vector3( float, float, float )>();
		type["X"] = sol::property( &Vector3::X, &Vector3::X );
		type["Y"] = sol::property( &Vector3::Y, &Vector3::Y );
		type["Z"] = sol::property( &Vector3::Z, &Vector3::Z );
		RegisterVectorFunctions( type );
	}

	void Scriptable<Vector4>::RegisterType( ScriptEngine& a_ScriptEngine )
	{
		auto type = a_ScriptEngine.RegisterNewType<Vector4>( "Vector4" );
		type["new"] = sol::constructors<Vector4(), Vector4( float ), Vector4( float, float, float, float )>();
		type["X"] = sol::property( &Vector4::X, &Vector4::X );
		type["Y"] = sol::property( &Vector4::Y, &Vector4::Y );
		type["Z"] = sol::property( &Vector4::Z, &Vector4::Z );
		type["W"] = sol::property( &Vector4::W, &Vector4::W );
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
			+[](const Rotator& a_Rotator) -> float { return glm::degrees(a_Rotator.Euler.X); },
			+[]( Rotator& a_Rotator, float a_Value ) { a_Rotator.Euler.X = glm::radians( a_Value ); } );

		type["Yaw"] = sol::property(
			+[]( const Rotator& a_Rotator ) -> float { return glm::degrees( a_Rotator.Euler.Y ); },
			+[]( Rotator& a_Rotator, float a_Value ) { a_Rotator.Euler.Y = glm::radians( a_Value ); } );

		type["Roll"] = sol::property(
			+[]( const Rotator& a_Rotator ) -> float { return glm::degrees( a_Rotator.Euler.Z ); },
			+[]( Rotator& a_Rotator, float a_Value ) { a_Rotator.Euler.Z = glm::radians( a_Value ); } );

		type["Forward"] = sol::property(
			+[]( const Rotator& a_Rotator ) -> Vector3 { return a_Rotator.GetForward(); },
			+[]( Rotator& a_Rotator, const Vector3& a_Value ) { a_Rotator.SetFromQuaternion( Math::QuatLookAt( a_Value, Vector3::Up() ) ); } );

		type["Right"] = sol::property(
			+[]( const Rotator& a_Rotator ) -> Vector3 { return a_Rotator.GetRight(); },
			+[]( Rotator& a_Rotator, const Vector3& a_Value ) { a_Rotator.SetFromQuaternion( Math::QuatLookAt( Vector3::Right(), a_Value) ); });

		type["Up"] = sol::property(
			+[]( const Rotator& a_Rotator ) -> Vector3 { return a_Rotator.GetUp(); },
			+[]( Rotator& a_Rotator, const Vector3& a_Value ) { a_Rotator.SetFromQuaternion( Math::QuatLookAt( Vector3::Forward(), a_Value) ); });
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