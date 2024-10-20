#include <Tridium.h>
#include <Tridium/IO/SceneSerializer.h>
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Editor/Panels/Asset/MeshSourceImporterPanel.h>
#include <Tridium/Rendering/SceneRenderer.h>

struct PersonInfo
{
	int Age = 0;
	std::string Name = "John Doe";
	float Height = 0.0f;
	bool IsMale = true;

	std::vector<std::string> Numbers = { "1", "2", "3" };
	std::map<std::string, std::string> Map = { { "Key1", "Value1" }, { "Key2", "Value2" } };
};

BEGIN_REFLECT( PersonInfo )
	PROPERTY( Age, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Name, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Height, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( IsMale, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Numbers, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Map, FLAGS( Serialize, EditAnywhere ) )
END_REFLECT( PersonInfo )

class Person : public Tridium::Component
{
	REFLECT(Person);
public:
	PersonInfo Info;
	std::map<std::string, PersonInfo> Children;
	std::vector<int> Numbers = { 1, 2, 3 };
};

BEGIN_REFLECT_COMPONENT( Person )
	PROPERTY( Info, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Children, FLAGS( Serialize, EditAnywhere ) )
	PROPERTY( Numbers, FLAGS( Serialize, EditAnywhere ) )
END_REFLECT( Person )

using namespace Tridium;

class TestLayer : public Layer
{
	virtual void OnUpdate() override
	{
		Time += Time::DeltaTime();
		if ( EnableRotatingLight )
		{
			s_LightDirection.x = glm::sin( Time );
		}
	}

	virtual void OnImGuiDraw() override
	{
		if ( ImGui::Begin( "Light Settings" ) )
		{
			ImGui::ColorEdit3( "Ambient Color", &s_AmbientColor.x );
			ImGui::SliderFloat3( "Light Direction", &s_LightDirection.x, -1.0f, 1.0f );
			ImGui::ColorEdit3( "Light Color", &s_LightColor.x );
			ImGui::DragFloat( "Light Intensity", &s_LightIntensity );
			ImGui::Checkbox( "Enable Rotating Light", &EnableRotatingLight );
		}

		ImGui::End();
	}
	float Time = 0.0f;
	bool EnableRotatingLight = false;
	std::string FilePath{ "Content/troll/TrollApose_low.fbx" };
};

class CreateNewAssetLayer : public Layer
{
	virtual void OnImGuiDraw() override
	{
		if ( !ImGui::Begin( "Create Asset" ) )
		{
			ImGui::End();
			return;
		}

		if ( ImGui::BeginCombo( "Asset Type", AssetTypeToString( AssetType ) ) )
		{
			if ( ImGui::Selectable( "None" ) )
				AssetType = EAssetType::None;

			if ( ImGui::Selectable( "Material" ) )
				AssetType = EAssetType::Material;

			ImGui::EndCombo();
		}

		if ( ImGui::Button( "Create" ) )
		{
			auto assetManager = AssetManager::Get<Editor::EditorAssetManager>();

			switch ( AssetType )
			{
			case EAssetType::Material:
			{
				SharedPtr<Material> material = MakeShared<Material>();

				AssetMetaData metaData;
				metaData.Handle = AssetHandle::Create();
				metaData.Path = assetManager->GetAbsolutePath( IO::FilePath("Content/Default.tmat") );
				metaData.AssetType = AssetType;
				metaData.Name = "Default";
				metaData.IsAssetLoaded = true;

				assetManager->CreateAsset( metaData, material );
			}
			break;
			default:
				break;
			}
		}

		ImGui::End();
	}

	EAssetType AssetType = EAssetType::None;
};

class ImportLayer : public Layer
{
	virtual void OnImGuiDraw() override
	{
		if ( !ImGui::Begin( "Import Asset" ) )
		{
			ImGui::End();
			return;
		}

		ImGui::InputText( "FilePath ", &FilePath );

		if ( ImGui::Button( "Import" ) )
		{
			IO::FilePath path( FilePath );
			if ( GetAssetTypeFromFileExtension( path.GetExtension() ) == EAssetType::MeshSource )
				Application::Get().PushOverlay( new Editor::MeshSourceImporterPanel( FilePath ) );
			else
				Tridium::AssetManager::Get<Tridium::Editor::EditorAssetManager>()->ImportAsset( FilePath );
		}

		ImGui::End();
	}

	std::string FilePath;
};


class MaterialEditorLayer : public Layer
{
	virtual void OnImGuiDraw() override
	{
		if ( !ImGui::Begin( "Material Editor" ) )
		{
			ImGui::End();
			return;
		}

		auto handle = MaterialHandle.ID();
		if ( ImGui::InputScalar( "Material Handle", ImGuiDataType_U64, &handle ) )
		{
			MaterialHandle = handle;
		}

		if ( SharedPtr<Material> material = AssetManager::GetAsset<Material>( MaterialHandle ) )
		{
			handle = material->AlbedoTexture.ID();
			if ( ImGui::InputScalar( "Albedo Texture Handle", ImGuiDataType_U64, &handle ) )
			{
				material->AlbedoTexture = handle;
			}
			handle = material->MetallicTexture.ID();
			if ( ImGui::InputScalar( "Metallic Texture Handle", ImGuiDataType_U64, &handle ) )
			{
				material->MetallicTexture = handle;
			}
			handle = material->RoughnessTexture.ID();
			if ( ImGui::InputScalar( "Roughness Texture Handle", ImGuiDataType_U64, &handle ) )
			{
				material->RoughnessTexture = handle;
			}
			handle = material->SpecularTexture.ID();
			if ( ImGui::InputScalar( "Specular Texture Handle", ImGuiDataType_U64, &handle ) )
			{
				material->SpecularTexture = handle;
			}
			handle = material->NormalTexture.ID();
			if ( ImGui::InputScalar( "Normal Texture Handle", ImGuiDataType_U64, &handle ) )
			{
				material->NormalTexture = handle;
			}
			handle = material->OpacityTexture.ID();
			if ( ImGui::InputScalar( "Opacity Texture Handle", ImGuiDataType_U64, &handle ) )
			{
				material->OpacityTexture = handle;
			}
			handle = material->EmissiveTexture.ID();
			if ( ImGui::InputScalar( "Emissive Texture Handle", ImGuiDataType_U64, &handle ) )
			{
				material->EmissiveTexture = handle;
			}
			handle = material->AOTexture.ID();
			if ( ImGui::InputScalar( "AO Texture Handle", ImGuiDataType_U64, &handle ) )
			{
				material->AOTexture = handle;
			}
		}

		ImGui::End();
	}

	AssetHandle MaterialHandle;
};

class SandboxGameInstance : public Tridium::GameInstance
{
	virtual void Init() override
	{
		Tridium::Application::Get().PushOverlay( new TestLayer() );
		Tridium::Application::Get().PushOverlay( new CreateNewAssetLayer() );
		Tridium::Application::Get().PushOverlay( new ImportLayer() );
		Tridium::Application::Get().PushOverlay( new MaterialEditorLayer() );
	}
};

Tridium::GameInstance* Tridium::CreateGameInstance()
{
	return new SandboxGameInstance();
}