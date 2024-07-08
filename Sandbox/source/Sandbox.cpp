#include <Tridium.h>
using namespace Tridium;
#include <Tridium/Rendering/Material.h>
#include <Tridium/IO/MaterialSerializer.h>
#include <Tridium/IO/SceneSerializer.h>

class ExampleLayer : public Tridium::Layer
{

public:

	ExampleLayer() = default;

	virtual void OnImGuiDraw() override
	{
		//ImGui::ShowDemoWindow();
	}
};

class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{
		SetScene();

		PushLayer( new ExampleLayer() );
	}

	~Sandbox()
	{

	}

	void SetScene()
	{
		//Ref<Material> mat = MakeRef<Material>();
		//mat->_SetHandle( MaterialHandle::Create() );
		//mat->SetShader( ShaderLibrary::GetShaderHandle( "Default" ) );
		//auto tex = TextureLoader::Import( "Content/Temp/stray-robot/textures/Stray_robotic_Material.009_BaseColor.png" );
		//tex->_SetHandle( TextureHandle::Create() );
		//TextureLibrary::AddTexture( tex->GetPath(), tex );
		//mat->BaseColorTexture = tex->GetHandle();
		//MaterialSerializer ser( mat );
		//ser.SerializeText( "Content/Robot.tasset" );

	#ifndef IS_EDITOR
			SceneSerializer serializer( Application::GetScene() );
			serializer.DeserializeText( ( Application::GetAssetDirectory() / "Scene.tridium" ).string() );
	#endif // !IS_EDITOR
	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}