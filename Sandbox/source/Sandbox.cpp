#include <Tridium.h>
using namespace Tridium;

class PlayerUI : public Tridium::Layer
{
public:
	PlayerUI()
		: Layer( "Example" ) {}

	void OnUpdate() override
	{
	}

	virtual void OnAttach() override
	{
	}

	virtual void OnImGuiDraw() override
	{
		if ( !Player.IsValid() )
			return;

		if ( !Player.HasComponent<LuaScriptComponent>() )
			return;

		auto& playerScript = Player.GetComponent<LuaScriptComponent>();

		auto playerMaxHealth = playerScript[ "MaxHealth" ];
		auto playerCurrentHealth = playerScript[ "CurrentHealth" ];

		if ( playerMaxHealth.valid() && playerCurrentHealth.valid() )
		{
			ImGui::SetNextWindowSize( { 200, 40 } );
			ImGui::Begin( "Health", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground );
			ImGui::ProgressBar( playerCurrentHealth.get<float>() / playerMaxHealth.get<float>() );
			ImGui::End();
		}
		Input::SetInputMode( EInputMode::Cursor, EInputModeValue::Cursor_Disabled );
	}

	virtual void OnEvent( Event& e )
	{
		EventDispatcher dispatcher( e );
		dispatcher.Dispatch<KeyPressedEvent>( TE_BIND_EVENT_FN( PlayerUI::OnKeyPressed, std::placeholders::_1 ) );
	}

	bool OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.GetKeyCode() == Input::KEY_ESCAPE )
		{
			Application::Quit();
			return true;
		}
	}

	GameObject Player;
};

class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{
		auto gameUI = new PlayerUI();
		PushOverlay( gameUI );

		auto& dangerCube = GetScene()->InstantiateGameObject( "Cube" );
		dangerCube.AddComponent<MeshComponent>();

		auto& player = GetScene()->InstantiateGameObject( "Player" );
		player.AddComponent<CameraComponent>();
		player.AddComponent<CameraControllerComponent>();
		player.AddComponent<LuaScriptComponent>( Script::Create( Application::GetAssetDirectory() / "Scripts/Player.lua" ) );

		gameUI->Player = player;

	}

	~Sandbox()
	{

	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}