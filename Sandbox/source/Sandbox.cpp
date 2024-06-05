#include <Tridium.h>
using namespace Tridium;




class PlayerUI : public Tridium::Layer
{
public:
	PlayerUI()
		: Layer( "PlayerUI" ) {}

	void OnUpdate() override
	{
	}

	virtual void OnAttach() override
	{
	}

	virtual void OnImGuiDraw() override
	{
		if ( !Player.IsValid() )
		{
			Player = Application::GetScene()->GetRegistry().view<CameraComponent>().front();
		}

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

	void SetScene()
	{
		auto& background = Application::GetScene()->InstantiateGameObject( "Background" );
		background.AddComponent<SpriteComponent>( ( Application::GetAssetDirectory() / "Engine/Editor/Icons/DeleteThisLater.png" ).string() );
		background.TryGetComponent<TransformComponent>()->Position.z = -30;
		background.TryGetComponent<TransformComponent>()->Scale = Vector3( 30 );

		auto& obstacleSpawner = Application::GetScene()->InstantiateGameObject( "Obstacle Spawner" );
		obstacleSpawner.AddComponent<MeshComponent>();
		obstacleSpawner.AddComponent<LuaScriptComponent>( Script::Create( Application::GetAssetDirectory() / "Scripts/Game/ObstacleSpawner.lua" ) );
		obstacleSpawner.TryGetComponent<TransformComponent>()->Position.z = -20;

		auto& player = Application::GetScene()->InstantiateGameObject( "Player" );
		player.AddComponent<CameraComponent>();
		player.AddComponent<SphereColliderComponent>().Radius *= 0.5f;
		auto& playerScript = player.AddComponent<LuaScriptComponent>( Script::Create( Application::GetAssetDirectory() / "Scripts/Game/Player.lua" ) );

		Player = player;
	}

	static PlayerUI* Get()
	{
		static PlayerUI* s_Instance = new PlayerUI();
		return s_Instance;
	}
};

class GameOverUI : public Tridium::Layer
{
public:
	GameOverUI()
		: Layer( "GameOver" ) {}

	void OnUpdate() override
	{
	}

	virtual void OnAttach() override
	{
	}

	virtual void OnImGuiDraw() override
	{
		if ( IsGameOver )
			ImGui::OpenPopup( " - GAME OVER! - " );

		if ( ImGui::BeginPopupModal( " - GAME OVER! - " ) )
		{
			if ( ImGui::Button( "Retry?" ) )
			{
				IsGameOver = false;
				Application::GetScene()->Clear();
				PlayerUI::Get()->SetScene();
				PlayerUI::Get()->Player.GetComponent<LuaScriptComponent>().GetEnvironment().set_function( "OnGameOver", GameOverUI::GameOver );

				ImGui::CloseCurrentPopup();
			}

			if ( ImGui::Button( "Quit?" ) )
			{
				Application::Quit();

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	bool IsGameOver = false;

	static void GameOver()
	{
		Get()->IsGameOver = true;
	}

	static GameOverUI* Get()
	{
		static GameOverUI* s_Instance = new GameOverUI();
		return s_Instance;
	}
};

class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{
		PushOverlay( GameOverUI::Get() );

		PlayerUI* gameUI = new PlayerUI();
		gameUI->SetScene();
		gameUI->Player.GetComponent<LuaScriptComponent>().GetEnvironment().set_function( "OnGameOver", GameOverUI::GameOver );
		PushOverlay( gameUI );
	}

	~Sandbox()
	{

	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}