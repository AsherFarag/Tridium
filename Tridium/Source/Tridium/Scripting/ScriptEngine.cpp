#include "tripch.h"
#include "ScriptEngine.h"

namespace Tridium {

    ScriptEngine* ScriptEngine::s_Instance = new ScriptEngine();

    ScriptEngine* ScriptEngine::Get()
    {
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
        m_State.open_libraries( sol::lib::base, sol::lib::math, sol::lib::package );
    }

}