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
        m_LuaState.open_libraries( sol::lib::base, sol::lib::math, sol::lib::package, sol::lib::io, sol::lib::debug );

        #pragma region Maths

        m_LuaState.new_usertype<Vector2>(
            "Vector2",
            sol::constructors<Vector2(), Vector2( float ), Vector2( float, float )>(),
            "x", sol::property( &Vector2::x, &Vector2::x ),
            "y", sol::property( &Vector2::y, &Vector2::y ) );

        m_LuaState.new_usertype<Vector3>(
            "Vector3",
            sol::constructors<Vector3(), Vector3( float ), Vector3( float, float, float )>(), 
            "x", sol::property( &Vector3::x, &Vector3::x ),
            "y", sol::property( &Vector3::y, &Vector3::y ),
            "z", sol::property( &Vector3::z, &Vector3::z ) );

        #pragma endregion

        m_LuaState.new_usertype<GameObject>(
            "GameObject",
            sol::factories( [](){ return Application::GetScene()->InstantiateGameObject(); } ),
            "ID", sol::property( &GameObject::ID ),
            "GetTransform", &GameObject::GetComponent<TransformComponent> );

        m_LuaState["GameObject"]["Destroy"] = []( GameObject& go ) { go.Destroy(); };

        m_LuaState.new_usertype<Component>(
            "Component",
            sol::no_constructor,
            "GetGameObject", &Component::GetGameObject );

        m_LuaState.new_usertype<TransformComponent>(
            "Transform",
            sol::no_constructor,
            sol::base_classes, sol::bases<Component>(),
            "Position", sol::property( &TransformComponent::Position, &TransformComponent::Position ),
            "Rotation", sol::property( &TransformComponent::Rotation, &TransformComponent::Rotation ),
            "Scale", sol::property( &TransformComponent::Scale, &TransformComponent::Scale ) );

        m_LuaState[ "Quit" ] = [] { Application::Quit(); };
    }

    void ScriptEngine::Recompile()
    {
        auto start = std::chrono::high_resolution_clock::now();

        auto& scriptComponents = Application::GetScene()->GetRegistry().view<LuaScriptComponent>();
        scriptComponents.each( []( auto entity, LuaScriptComponent& sc )
            {
                sc.Compile();
            } );

        std::chrono::duration<double> elapsed_seconds = std::chrono::high_resolution_clock::now() - start;
        TE_CORE_INFO( "SCRIPTS RECOMPILED - Time Taken: {0} seconds", elapsed_seconds.count() < 0.01 ? 0.0 : elapsed_seconds.count() );
    }

}