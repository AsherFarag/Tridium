#include <Tridium.h>
#include <Tridium/IO/SceneSerializer.h>
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Editor/Panels/Asset/MeshSourceImporterPanel.h>

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

class TestLayer : public Tridium::Layer
{
	virtual void OnImGuiDraw() override
	{
		using namespace Tridium;

		if ( ImGui::Begin( "Import Asset" ) )
		{
			ImGui::InputText( "FilePath ", &FilePath );

			if ( ImGui::Button( "Import" ) )
			{
				//ImportedAssetHandle = Tridium::AssetManager::Get<Tridium::Editor::EditorAssetManager>()->ImportAsset( FilePath );
				//Tridium::AssetManager::GetAsset<Tridium::Texture>( ImportedAssetHandle );
				Application::Get().PushOverlay( new Editor::MeshSourceImporterPanel( FilePath ) );
			}

			ImGui::InputScalar( "AssetHandle", ImGuiDataType_U64, &ImportedAssetHandle, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly );
		}

		ImGui::End();

		if ( ImGui::Begin( "Create Mesh" ) )
		{
			if ( ImGui::InputScalar( "Mesh Source", ImGuiDataType_U64, &MeshSourceHandle, nullptr, nullptr, nullptr, ImGuiInputTextFlags_EnterReturnsTrue ) )
			{
				SharedPtr<StaticMesh> mesh = MakeShared<StaticMesh>( MeshSourceHandle );
				MeshHandle = AssetHandle::Create();
				AssetManager::Get<Editor::EditorAssetManager>()->AddMemoryOnlyAsset( MeshHandle, mesh);
			}

			ImGui::InputScalar( "AssetHandle", ImGuiDataType_U64, &MeshHandle, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly );
		}

		ImGui::End();
	}


	Tridium::AssetHandle ImportedAssetHandle;
	std::string FilePath{ "Content/Mutant Punch.fbx" };


	Tridium::AssetHandle MeshSourceHandle;
	Tridium::AssetHandle MeshHandle;
};

class SandboxGameInstance : public Tridium::GameInstance
{
	virtual void Init() override
	{
		Tridium::Application::Get().PushOverlay( new TestLayer() );
	}
};

Tridium::GameInstance* Tridium::CreateGameInstance()
{
	return new SandboxGameInstance();
}