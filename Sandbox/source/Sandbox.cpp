#include <Tridium.h>
using namespace Tridium;
#include <Tridium/Rendering/Material.h>
#include <Tridium/IO/MaterialSerializer.h>
#include <Tridium/IO/SceneSerializer.h>
#include <variant>
#include "Editor/Editor.h"

#include <Tridium/Asset/AssetMetaData.h>

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

		//AssetRef<Material> mat(new Material());
		//mat->AddProperty( "Color", { EPropertyType::Color, Color{} } );
		//mat->AddProperty( "Index", { EPropertyType::Int, 5 } );
		//mat->AddProperty( "IntArr", { EPropertyType::IntArray, std::vector<int>{1,2,3,4,5,6,7,8,9,0} } );
		//mat->AddProperty( "MatArr", { EPropertyType::Matrix4Array, std::vector<Matrix4>{Matrix4{1}, Matrix4{2}} } );

		//MaterialSerializer s( mat );
		//s.SerializeText( "Content/testMat.tmat" );

		AssetRef<Material> newmat( new Material() );
		MaterialSerializer ser( newmat );
		ser.DeserializeText( "Content/testMat.tmat" );

		auto ide = entt::resolve<ModelMetaData>();
		
		int i = 0;
		for ( auto&& [id, prop] : entt::resolve<ModelImportSettings>().prop() ) {
			i++;
		}

		LOG_INFO( i );
		//
		//for ( auto&& [id, type] : entt::resolve() ) {
		//	LOG_INFO( type.info().name().data() );
		//}
		//Editor::MaterialEditorPanel* panel = Editor::GetEditorLayer()->PushPanel<Editor::MaterialEditorPanel>();
		//panel->SetMaterial( newmat );
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