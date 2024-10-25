#include <Tridium.h>
#include <Tridium/IO/SceneSerializer.h>
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Editor/Panels/Asset/MeshSourceImporterPanel.h>
#include <Tridium/Rendering/SceneRenderer.h>
#include <Tridium/Asset/AssetFactory.h>

using namespace Tridium;

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

		ImGui::InputText( "Asset Path", &AssetPath );

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
				metaData.Path = AssetPath;
				metaData.AssetType = AssetType;
				metaData.Name = metaData.Path.GetFilenameWithoutExtension();
				metaData.IsAssetLoaded = true;

				assetManager->CreateAsset( metaData, material );
				AssetFactory::SaveAsset( metaData, material );
			}
			break;
			default:
				break;
			}
		}

		ImGui::End();
	}

	EAssetType AssetType = EAssetType::None;
	std::string AssetPath;
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

		ImGui::SameLine();

		if ( ImGui::ArrowButton( "Browse", ImGuiDir_Down ) )
			ImGui::OpenPopup( "ImportAssetPopup" );

		if ( ImGui::BeginPopup( "ImportAssetPopup" ) )
		{
			auto assetmanager = AssetManager::Get<Editor::EditorAssetManager>();
			for ( auto entry : std::filesystem::recursive_directory_iterator( Application::GetActiveProject()->GetAssetDirectory().ToString() ) )
			{
				if ( entry.is_directory() )
					continue;

				if ( assetmanager->GetAssetMetaData( entry.path() ).IsValid() )
					continue;

				std::string asString = entry.path().generic_string();
				if ( ImGui::Selectable( asString.c_str() ) )
				{
					FilePath = asString;
					ImGui::CloseCurrentPopup();
					break;
				}
			}

			ImGui::EndPopup();
		}

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

bool DrawAssetHandle( const char* a_Name, AssetHandle& a_Value, EAssetType a_Type )
{
	bool modified = false;

	auto assetManager = AssetManager::Get<Editor::EditorAssetManager>();

	const char* assetName = "None";
	if ( a_Value.Valid() )
		assetName = assetManager->GetAssetMetaData( a_Value ).Name.c_str();

	if ( ImGui::BeginCombo( a_Name, assetName ) )
	{
		for ( const auto& [handle, assetMetaData] : assetManager->GetAssetRegistry().AssetMetaData )
		{
			if ( assetMetaData.AssetType != a_Type )
				continue;

			std::string name = !assetMetaData.Name.empty() ? assetMetaData.Name : assetMetaData.Path.ToString();
			ImGui::ScopedID id( handle.ID() );
			if ( ImGui::Selectable( name.c_str(), a_Value == handle ) )
			{
				a_Value = handle;
				modified = true;
				break;
			}

			if ( ImGui::BeginItemTooltip() )
			{
				ImGui::Text( "Asset Type: %s", AssetTypeToString( assetMetaData.AssetType ) );
				ImGui::Text( "Path: %s", assetMetaData.Path.ToString().c_str() );

				ImGui::EndTooltip();
			}
		}

		ImGui::EndCombo();
	}

	return modified;
}


class MaterialEditorLayer : public Layer
{
	virtual void OnImGuiDraw() override
	{
		if ( !ImGui::Begin( "Material Editor" ) )
		{
			ImGui::End();
			return;
		}

		DrawAssetHandle( "Material", MaterialHandle, EAssetType::Material );

		if ( SharedPtr<Material> material = AssetManager::GetAsset<Material>( MaterialHandle ) )
		{
			DrawAssetHandle( "Albedo Texture", material->AlbedoTexture, EAssetType::Texture );
			DrawAssetHandle( "Metallic Texture", material->MetallicTexture, EAssetType::Texture );
			DrawAssetHandle( "Roughness Texture", material->RoughnessTexture, EAssetType::Texture );
			DrawAssetHandle( "Specular Texture", material->SpecularTexture, EAssetType::Texture );
			DrawAssetHandle( "Normal Texture", material->NormalTexture, EAssetType::Texture );
			DrawAssetHandle( "Opacity Texture", material->OpacityTexture, EAssetType::Texture );
			DrawAssetHandle( "Emissive Texture", material->EmissiveTexture, EAssetType::Texture );
			DrawAssetHandle( "AO Texture", material->AOTexture, EAssetType::Texture );
		}

		ImGui::End();
	}

	AssetHandle MaterialHandle;
};

class SandboxGameInstance : public Tridium::GameInstance
{
	virtual void Init() override
	{
		Tridium::Application::Get().PushOverlay( new CreateNewAssetLayer() );
		Tridium::Application::Get().PushOverlay( new ImportLayer() );
		Tridium::Application::Get().PushOverlay( new MaterialEditorLayer() );
	}
};

Tridium::GameInstance* Tridium::CreateGameInstance()
{
	return new SandboxGameInstance();
}