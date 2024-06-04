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

        m_LuaState.new_usertype<Application>(
            "App",
            sol::no_constructor);
        m_LuaState[ "App" ][ "Quit" ] = []{ Application::Quit(); };

        m_LuaState.new_usertype<GameObject>(
            "GameObject",
            sol::factories( [](){ return Application::GetScene()->InstantiateGameObject(); } ),
            "ID", sol::property( &GameObject::ID ),
            "GetTransform", &GameObject::GetComponent<TransformComponent>,
			"GetSphereCollider", &GameObject::GetComponent<SphereColliderComponent>,
            "AddScript", []( GameObject& go, std::string& scriptPath ) { go.AddComponent<LuaScriptComponent>( scriptPath ); } );

        m_LuaState[ "GameObject" ][ "AddMesh" ] = []( GameObject& go ) { go.AddComponent<MeshComponent>(); };
		m_LuaState[ "GameObject" ][ "AddSphereCollider" ] = []( GameObject& go ) { go.AddComponent<SphereColliderComponent>(); };
		m_LuaState[ "GameObject" ][ "Destroy" ] = []( GameObject& go ) { go.Destroy(); };

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

		m_LuaState.new_usertype<SphereColliderComponent>(
			"SphereCollider",
			sol::no_constructor,
			sol::base_classes, sol::bases<Component>(),
			"Radius", sol::property( &SphereColliderComponent::Radius, &SphereColliderComponent::Radius ),
			"IsColliding", sol::property( &SphereColliderComponent::IsColliding, &SphereColliderComponent::IsColliding ) );

        m_LuaState.new_usertype<LuaScriptComponent>(
            "Script",
            sol::no_constructor,
            sol::base_classes, sol::bases<Component>());

        #pragma region MyRegion

		m_LuaState.new_usertype<Input>( "Input", sol::no_constructor );
		m_LuaState[ "Input" ][ "IsKeyDown" ] = []( int key ) { return Input::IsKeyPressed( key ); };

   //     m_LuaState.new_enum("InputKey",
			//"SPACE",	  (int)Input::KEY_SPACE,
			//"APOSTROPHE", (int)Input::KEY_APOSTROPHE,
			//"COMMA",	  (int)Input::KEY_COMMA,
			//"MINUS",	  (int)Input::KEY_MINUS, 
			//"PERIOD",	  (int)Input::KEY_PERIOD, 
			//"SLASH",	  (int)Input::KEY_SLASH, 
			//"0", (int)Input::KEY_0,
			//"1", (int)Input::KEY_1,
			//"2", (int)Input::KEY_2,
			//"3", (int)Input::KEY_3,
			//"4", (int)Input::KEY_4,
			//"5", (int)Input::KEY_5,
			//"6", (int)Input::KEY_6,
			//"7", (int)Input::KEY_7,
			//"8", (int)Input::KEY_8,
			//"9", (int)Input::KEY_9,
			//"SEMICOLON", (int)Input::KEY_SEMICOLON,
			//"EQUAL", (int)Input::KEY_EQUAL,
			//"A", (int)Input::KEY_A,
			//"B", (int)Input::KEY_B,
			//"C", (int)Input::KEY_C,
			//"D", (int)Input::KEY_D,
			//"E", (int)Input::KEY_E,
			//"F", (int)Input::KEY_F,
			//"G", (int)Input::KEY_G,
			//"H", (int)Input::KEY_H,
			//"I", (int)Input::KEY_I,
			//"J", (int)Input::KEY_J,
			//"K", (int)Input::KEY_K,
			//"L", (int)Input::KEY_L,
			//"M", (int)Input::KEY_M,
			//"N", (int)Input::KEY_N,
			//"O", (int)Input::KEY_O,
			//"P", (int)Input::KEY_P,
			//"Q", (int)Input::KEY_Q,
			//"R", (int)Input::KEY_R,
			//"S", (int)Input::KEY_S,
			//"T", (int)Input::KEY_T,
			//"U", (int)Input::KEY_U,
			//"V", (int)Input::KEY_V,
			//"W", (int)Input::KEY_W,
			//"X", (int)Input::KEY_X,
			//"Y", (int)Input::KEY_Y,
			//"Z", (int)Input::KEY_Z,
			//"LEFT_BRACKET", (int)Input::KEY_LEFT_BRACKET,
			//"BACKSLASH", (int)Input::KEY_BACKSLASH,
			//"RIGHT_BRACKET", (int)Input::KEY_RIGHT_BRACKET,
			//"GRAVE_ACCENT", (int)Input::KEY_GRAVE_ACCENT,
			//"WORLD_1", (int)Input::KEY_WORLD_1,
			//"WORLD_2", (int)Input::KEY_WORLD_2,
			//"ESCAPE", (int)Input::KEY_ESCAPE,
			//"ENTER", (int)Input::KEY_ENTER,
			//"TAB", (int)Input::KEY_TAB,
			//"BACKSPACE", (int)Input::KEY_BACKSPACE,
			//"INSERT", (int)Input::KEY_INSERT,
			//"DELETE", (int)Input::KEY_DELETE,
			//"RIGHT", (int)Input::KEY_RIGHT,
			//"LEFT", (int)Input::KEY_LEFT,
			//"DOWN", (int)Input::KEY_DOWN,
			//"UP", (int)Input::KEY_UP,
			//"PAGE_UP", (int)Input::KEY_PAGE_UP,
			//"PAGE_DOWN", (int)Input::KEY_PAGE_DOWN,
			//"HOME", (int)Input::KEY_HOME,
			//"END", (int)Input::KEY_END,
			//"CAPS_LOCK", (int)Input::KEY_CAPS_LOCK,
			//"SCROLL_LOCK", (int)Input::KEY_SCROLL_LOCK,
			//"NUM_LOCK", (int)Input::KEY_NUM_LOCK,
			//"PRINT_SCREEN", (int)Input::KEY_PRINT_SCREEN,
			//"PAUSE", (int)Input::KEY_PAUSE,
			//"F1", (int)Input::KEY_F1,
			//"F2", (int)Input::KEY_F2,
			//"F3", (int)Input::KEY_F3,
			//"F4", (int)Input::KEY_F4,
			//"F5", (int)Input::KEY_F5,
			//"F6", (int)Input::KEY_F6,
			//"F7", (int)Input::KEY_F7,
			//"F8", (int)Input::KEY_F8,
			//"F9", (int)Input::KEY_F9,
			//"F10", (int)Input::KEY_F10,
			//"F11", (int)Input::KEY_F11,
			//"F12", (int)Input::KEY_F12,
			//"F13", (int)Input::KEY_F13,
			//"F14", (int)Input::KEY_F14,
			//"F15", (int)Input::KEY_F15,
			//"F16", (int)Input::KEY_F16,
			//"F17", (int)Input::KEY_F17,
			//"F18", (int)Input::KEY_F18,
			//"F19", (int)Input::KEY_F19,
			//"F20", (int)Input::KEY_F20,
			//"F21", (int)Input::KEY_F21,
			//"F22", (int)Input::KEY_F22,
			//"F23", (int)Input::KEY_F23,
			//"F24", (int)Input::KEY_F24,
			//"F25", (int)Input::KEY_F25,
			//"KP_0", (int)Input::KEY_KP_0,
			//"KP_1", (int)Input::KEY_KP_1,
			//"KP_2", (int)Input::KEY_KP_2,
			//"KP_3", (int)Input::KEY_KP_3,
			//"KP_4", (int)Input::KEY_KP_4,
			//"KP_5", (int)Input::KEY_KP_5,
			//"KP_6", (int)Input::KEY_KP_6,
			//"KP_7", (int)Input::KEY_KP_7,
			//"KP_8", (int)Input::KEY_KP_8,
			//"KP_9", (int)Input::KEY_KP_9,
			//"KP_DECIMAL", (int)Input::KEY_KP_DECIMAL,
			//"KP_DIVIDE", (int)Input::KEY_KP_DIVIDE,
			//"KP_MULTIPLY", (int)Input::KEY_KP_MULTIPLY,
			//"KP_SUBTRACT", (int)Input::KEY_KP_SUBTRACT,
			//"KP_ADD", (int)Input::KEY_KP_ADD,
			//"KP_ENTER", (int)Input::KEY_KP_ENTER,
			//"KP_EQUAL", (int)Input::KEY_KP_EQUAL,
			//"LEFT_SHIFT", (int)Input::KEY_LEFT_SHIFT,
			//"LEFT_CONTROL", (int)Input::KEY_LEFT_CONTROL,
			//"LEFT_ALT", (int)Input::KEY_LEFT_ALT,
			//"LEFT_SUPER", (int)Input::KEY_LEFT_SUPER,
			//"RIGHT_SHIFT", (int)Input::KEY_RIGHT_SHIFT,
			//"RIGHT_CONTROL", (int)Input::KEY_RIGHT_CONTROL,
			//"RIGHT_ALT", (int)Input::KEY_RIGHT_ALT,
			//"RIGHT_SUPER", (int)Input::KEY_RIGHT_SUPER,
			//"MENU", (int)Input::KEY_MENU
   //     );

        #pragma endregion

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