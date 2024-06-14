#include <Tridium.h>
using namespace Tridium;

DEFINE_COMPONENT( Test, ScriptableComponent )
{
	virtual void OnUpdate() override
	{
		std::cout << "Cpp" << std::endl;
	}
};

class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{
		SetScene();
	}

	~Sandbox()
	{

	}

	void SetScene()
	{
		// TEMP
		std::string vertexSrc =
			R"(
						#version 410

						layout(location = 0) in vec3 aPosition;
						layout(location = 1) in vec4 aColor;
						
						out vec4 vPosition;
						out vec4 vColor;			
						
						uniform mat4 uPVM;
						
						void main()
						{	
							gl_Position = uPVM * vec4(aPosition, 1);
							vPosition =  vec4(aPosition, 1);
							vColor = aColor;
						}
					)";

		std::string fragSrc =
			R"(
						#version 410 core
						
						layout(location = 0) out vec4 aColor;

						in vec4 vPosition;
						in vec4 vColor;						
						
						uniform vec4 uColour;

						void main()
						{
							aColor = (vColor * uColour);
						}
					)";

		Shader::Create( "Default", vertexSrc, fragSrc );

		// TEMP
		vertexSrc =
			R"(
						#version 410

						layout(location = 0) in vec3 aPosition;
						layout(location = 1) in vec2 aTextureCoords;
						
						out vec4 vPosition;
						out vec2 vTextureCoords;			
						
						uniform mat4 uPVM;
						
						void main()
						{	
							gl_Position = uPVM * vec4(aPosition, 1);
							vPosition =  vec4(aPosition, 1);
							vTextureCoords = aTextureCoords;
						}
					)";

		fragSrc =
			R"(
						#version 410 core

						out vec4 aFragColour;

						in vec4 vPosition;
						in vec2 vTextureCoords;						
						
						uniform sampler2D uTexture;

						void main()
						{
							aFragColour = vec4(texture(uTexture, vTextureCoords).rgb, 1);
						}
					)";

		Shader::Create( "Texture", vertexSrc, fragSrc );

		//auto& background = GetScene()->InstantiateGameObject( "Background" );
		//background.AddComponent<SpriteComponent>( ( Application::GetAssetDirectory() / "Engine/Editor/Icons/DeleteThisLater.png" ).string() );
		//background.TryGetComponent<TransformComponent>()->Position.z = -30;
		//background.TryGetComponent<TransformComponent>()->Scale = Vector3( 15 );

		//auto& obstacleSpawner = GetScene()->InstantiateGameObject( "Obstacle Spawner" );
		//obstacleSpawner.AddComponent<MeshComponent>();
		//obstacleSpawner.AddComponent<LuaScriptComponent>( Script::Create( Application::GetAssetDirectory() / "Scripts/Game/ObstacleSpawner.lua" ) );
		//obstacleSpawner.TryGetComponent<TransformComponent>()->Position.z = -20;

		//auto& player = GetScene()->InstantiateGameObject( "Player" );
		//player.AddComponent<CameraComponent>();
		//player.AddComponent<LuaScriptComponent>( Script::Create( Application::GetAssetDirectory() / "Scripts/Game/Player.lua" ) );

		for ( size_t i = 0; i < 1000; i++ )
		{
			auto& go = GetScene()->InstantiateGameObject( "Cpp" );
			go.AddComponent<Test>();
		}
	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}