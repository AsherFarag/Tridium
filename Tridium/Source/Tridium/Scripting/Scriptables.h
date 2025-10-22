#pragma once
#include <Tridium/Math/Math.h>
#include <Tridium/Math/Rotator.h>
#include <Tridium/Graphics/Color.h>

// Forward declarations
namespace Tridium {
	class OldGameObject;
}
// --------------------

namespace Tridium {

	// Forward declaration
	class ScriptEngine;
	// -------------------


	template <typename T>
	struct Scriptable 
	{
		using Type = T;

		static void RegisterType( ScriptEngine& a_ScriptEngine )
		{
			//static_assert( false, "RegisterType must be specialized for this type" );
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// GameObject
	//////////////////////////////////////////////////////////////////////////

	template <>
	struct Scriptable<OldGameObject>
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
	struct Scriptable<Color4>
	{
		static void RegisterType( ScriptEngine& a_ScriptEngine );
	};

	//////////////////////////////////////////////////////////////////////////

} // namespace Tridium