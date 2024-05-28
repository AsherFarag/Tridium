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
            sol::no_constructor,
            "ID", sol::property( &GameObject::ID ),
            "GetTransform", &GameObject::GetTransform );

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

        //m_LuaState.script( "function Vector3:print() print('x: '.. self.x .. ' y: '.. self.y .. ' z: '.. self.z) end" );
        //m_LuaState.script( "local vec = Vector3.new(1.1, 1.2, 1.3) \n vec:print()" );

        //m_LuaState.script( "function Vector2:print() print('x: '.. self.x .. ' y: '.. self.y) end" );
        //m_LuaState.script( "local vec = Vector2.new(5.1, 2.2) \n vec:print()" );

        //m_LuaState[ "go" ] = Application::GetScene()->InstantiateGameObject();
        //m_LuaState.script( "go:GetTransform().Position.x = 1" );

        //// runtime additions: through the sol API
        //m_LuaState[ "Vector3" ][ "func" ] = []( Vector3& v ) { return v.x; };
        //// runtime additions: through a lua script
        //m_LuaState.script( "function object:print () print(self:func()) end" );

        //// see it work
        //m_LuaState.script( "local obj = object.new() \n obj:print()" );
    }

    void ScriptEngine::Recompile()
    {
        auto& lua = GetLuaState();

        auto& scriptComponents = Application::GetScene()->GetRegistry().view<LuaScriptComponent>();
        scriptComponents.each( [&]( auto entity, LuaScriptComponent& sc )
            {
                sc.Compile( lua );
            } );

        //for ( auto& script : scriptLib->m_Library )
        //{
        //    script.second->Recompile();
        //}
    }

}