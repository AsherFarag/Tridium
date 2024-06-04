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

		return false;
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


		auto& background = GetScene()->InstantiateGameObject( "Background" );
		background.AddComponent<SpriteComponent>();
		background.TryGetComponent<TransformComponent>()->Position.z = -30;
		//background.TryGetComponent<TransformComponent>()->Rotation.y = glm::radians( 90.f );
		background.TryGetComponent<TransformComponent>()->Scale = Vector3(15);

		auto& obstacleSpawner = GetScene()->InstantiateGameObject( "Obstacle Spawner" );
		obstacleSpawner.AddComponent<MeshComponent>();
		obstacleSpawner.AddComponent<LuaScriptComponent>( Script::Create( Application::GetAssetDirectory() / "Scripts/Game/ObstacleSpawner.lua" ) );
		obstacleSpawner.TryGetComponent<TransformComponent>()->Position.z = -20;

		auto& player = GetScene()->InstantiateGameObject( "Player" );
		player.AddComponent<CameraComponent>();
		player.AddComponent<LuaScriptComponent>( Script::Create( Application::GetAssetDirectory() / "Scripts/Game/Player.lua" ) );

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