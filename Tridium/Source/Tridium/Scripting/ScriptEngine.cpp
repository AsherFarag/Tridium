#include "tripch.h"
#include "ScriptEngine.h"

#include <Tridium/ECS/Components/Types.h>

namespace Tridium {

    ScriptEngine* ScriptEngine::Get()
    {
        static ScriptEngine* s_Instance = new ScriptEngine();

        return s_Instance;
    }

    ScriptEngine::ScriptEngine()
    {
        Init();
    }

    ScriptEngine::~ScriptEngine()
    {
    }

    void ScriptEngine::Init()
    {
        m_LuaState.open_libraries( sol::lib::base, sol::lib::math, sol::lib::package );

        m_LuaState.new_usertype<Vector3>( 
            sol::constructors<Vector3()>(),
            "x", sol::property( &Vector3::x ),
            "y", sol::property( & Vector3::y ),
            "z", sol::property( &Vector3::z ) );

        m_LuaState.new_usertype<TransformComponent>(
            sol::constructors<TransformComponent()>(),
            "Position", sol::property( &TransformComponent::Position ),
            "Rotation", sol::property( &TransformComponent::Position ),
            "Scale", sol::property( &TransformComponent::Position ) );
    }

}