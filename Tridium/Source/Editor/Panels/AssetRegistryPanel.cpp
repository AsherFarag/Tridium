#include "tripch.h"
#ifdef IS_EDITOR
#include "AssetRegistryPanel.h"
#include <Tridium/Asset/EditorAssetManager.h>

namespace Tridium::Editor {
	void AssetRegistryPanel::OnImGuiDraw()
	{
		if ( ImGui::Begin( m_Name.c_str() ) )
		{
			AssetRegistry& assetRegistry = AssetManager::Get<EditorAssetManager>()->m_AssetRegistry;

			for ( auto& [handle, metaData] : assetRegistry.AssetMetaData )
			{
				const std::string& assetName = metaData.Name.size() ? metaData.Name.c_str() : metaData.Path.GetFilename();
				if ( ImGui::TreeNode( 
							std::to_string( handle.ID() ).c_str(),
							"Asset: %s", assetName.c_str() ) )
				{
					const auto inputFlags = ImGuiInputTextFlags_ReadOnly;

					// ID
					auto assetID = handle.ID();
					ImGui::InputScalar( "Asset Handle", ImGuiDataType_U64, &assetID, nullptr, nullptr, nullptr, inputFlags );
					// Path
					std::string path = metaData.Path.ToString();
					ImGui::InputText( "Asset Path", &path, inputFlags );
					// Asset Type
					char* assetType = const_cast<char*>( AssetTypeToString( metaData.AssetType ) );
					ImGui::InputText( "Asset Type", assetType, 32, inputFlags );
					// Asset Loaded
					const char* assetLoaded = metaData.IsAssetLoaded ? "True" : "False";
					ImGui::InputText( "Asset Loaded", const_cast<char*>( assetLoaded ), inputFlags );

					ImGui::TreePop();
				}
			}
		}

		ImGui::End();
	}
}

#endif // IS_EDITOR