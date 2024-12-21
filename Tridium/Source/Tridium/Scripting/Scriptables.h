#pragma once
#include <Tridium/Math/Math.h>
#include <Tridium/Math/Rotator.h>
#include <Tridium/Core/Color.h>

// Forward declarations
namespace Tridium {
	class GameObject;
}
// --------------------

namespace Tridium::Script {

	// Forward declaration
	class ScriptEngine;
	// -------------------


	template <typename T>
	struct Scriptable 
	{
		using Type = T;

		static void RegisterType( ScriptEngine& a_ScriptEngine )
		{
			static_assert( false, "RegisterType must be specialized for this type" );
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// GameObject
	//////////////////////////////////////////////////////////////////////////

	template <>
	struct Scriptable<GameObject>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	//////////////////////////////////////////////////////////////////////////
	// Math
	//////////////////////////////////////////////////////////////////////////

	template <>
	struct Scriptable<Vector2>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	template <>
	struct Scriptable<Vector3>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	template <>
	struct Scriptable<Vector4>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	template <>
	struct Scriptable<iVector2>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	template <>
	struct Scriptable<iVector3>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	template <>
	struct Scriptable<iVector4>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	template <>
	struct Scriptable<Matrix2>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	template <>
	struct Scriptable<Matrix3>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	template <>
	struct Scriptable<Matrix4>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	template <>
	struct Scriptable<Rotator>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	template<>
	struct Scriptable<Color>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	//////////////////////////////////////////////////////////////////////////

} // End of namespace Tridium::Script