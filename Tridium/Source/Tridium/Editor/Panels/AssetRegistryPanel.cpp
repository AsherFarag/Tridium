#include "tripch.h"
#if IS_EDITOR
#include "AssetRegistryPanel.h"
#include <Tridium/Asset/EditorAssetManager.h>

namespace Tridium {

	void AssetRegistryPanel::OnImGuiDraw()
	{
		if ( ImGui::Begin( "Asset Registry") )
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
					if ( ImGui::BeginItemTooltip() )
					{
						ImGui::Text( path.c_str() );

						ImGui::EndTooltip();
					}

					// Asset Type
					char* assetType = const_cast<char*>( AssetTypeToString( metaData.AssetType ) );
					ImGui::InputText( "Asset Type", assetType, 32, inputFlags );

					// Asset Loaded
					const char* assetLoaded = metaData.IsAssetLoaded ? "True" : "False";
					ImGui::InputText( "Asset Loaded", const_cast<char*>( assetLoaded ), inputFlags );

					if ( auto it = assetRegistry.AssetDependencies.find( handle ); it != assetRegistry.AssetDependencies.end() )
					{
						// Dependencies
						if ( ImGui::TreeNode( "Dependencies" ) )
						{
							for ( auto& dependency : it->second )
							{
								auto dependencyID = dependency.ID();
								ImGui::PushID( dependencyID );
								ImGui::InputScalar( "", ImGuiDataType_U64, &dependencyID, nullptr, nullptr, nullptr, inputFlags );
								ImGui::PopID();
							}
							ImGui::TreePop();
						}
					}

					ImGui::TreePop();
				}
			}
		}

		ImGui::End();
	}
}

#endif // IS_EDITOR