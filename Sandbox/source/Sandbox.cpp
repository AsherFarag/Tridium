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


class MaterialEditorLayer : public Layer
{
	virtual void OnImGuiDraw() override
	{
		if ( !ImGui::Begin( "Material Editor" ) )
		{
			ImGui::End();
			return;
		}

		Editor::DrawProperty( "Material", MaterialHandle, Editor::EDrawPropertyFlags::Editable );

		if ( SharedPtr<Material> material = AssetManager::GetAsset<Material>( MaterialHandle ) )
		{
			DrawProperty( "Albedo Texture", material->AlbedoTexture, Editor::EDrawPropertyFlags::Editable );
			DrawProperty( "Metallic Texture", material->MetallicTexture, Editor::EDrawPropertyFlags::Editable );
			DrawProperty( "Roughness Texture", material->RoughnessTexture, Editor::EDrawPropertyFlags::Editable );
			DrawProperty( "Specular Texture", material->SpecularTexture, Editor::EDrawPropertyFlags::Editable );
			DrawProperty( "Normal Texture", material->NormalTexture, Editor::EDrawPropertyFlags::Editable );
			DrawProperty( "Opacity Texture", material->OpacityTexture, Editor::EDrawPropertyFlags::Editable );
			DrawProperty( "Emissive Texture", material->EmissiveTexture, Editor::EDrawPropertyFlags::Editable );
			DrawProperty( "AO Texture", material->AOTexture, Editor::EDrawPropertyFlags::Editable );
		}

		ImGui::End();
	}

	MaterialHandle MaterialHandle;
};

class SandboxGameInstance : public Tridium::GameInstance
{
	virtual void Init() override
	{
		Tridium::Application::Get().PushOverlay( new CreateNewAssetLayer() );
		Tridium::Application::Get().PushOverlay( new MaterialEditorLayer() );
	}
};

Tridium::GameInstance* Tridium::CreateGameInstance()
{
	return new SandboxGameInstance();
}