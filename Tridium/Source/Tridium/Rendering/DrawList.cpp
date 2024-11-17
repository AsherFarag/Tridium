#include "tripch.h"
#include "DrawList.h"
#include "Mesh.h"
#include <Tridium/Asset/AssetManager.h>

namespace Tridium {
	DrawList::DrawList()
	{
		// Iterates over all the draw passes and initializes the command list for each one
		for ( uint8_t i = BIT( 0 ); i < (uint8_t)EDrawPass::LAST; i <<= 1 )
		{
			m_CommandLists[(EDrawPass)i] = {};
		}
	}

	void DrawList::Clear()
	{
		for ( auto&& [pass, commandList]: m_CommandLists )
		{
			commandList.clear();
		}
	}

	void DrawList::AddCommand( DrawPass a_DrawPassFlags, StaticMeshHandle a_Mesh, const std::vector<MaterialHandle>& a_OverrideMaterials, const Matrix4& a_Transform )
	{
		SharedPtr<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>( a_Mesh );
		if ( !mesh )
			return;

		SharedPtr<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>( mesh->GetMeshSource() );
		if ( !meshSource )
			return;

		// For each submesh in the static mesh
		for ( uint32_t subMeshIndex : mesh->GetSubMeshes() )
		{
			const SubMesh& subMesh = meshSource->GetSubMesh( subMeshIndex );

			// Get the material that will be used, either from the override list or the mesh source
			MaterialHandle material = MaterialHandle::InvalidGUID;
			if ( IsValidIndex( a_OverrideMaterials, subMeshIndex ) )
			{
				material = a_OverrideMaterials[subMeshIndex];
			}
			else if ( IsValidIndex( meshSource->GetMaterials(), subMesh.MaterialIndex ) )
			{
				material = meshSource->GetMaterials()[subMesh.MaterialIndex];
			}

			DrawCommandKey drawCommandKey = { a_Mesh, subMeshIndex, material };
			// For each draw pass, add the command
			for ( auto&& [pass, commandList] : m_CommandLists )
			{
				if ( ( (DrawPass)pass & a_DrawPassFlags ) == 0 )
					continue;

				DrawCommand& drawCommand = commandList[drawCommandKey];
				drawCommand.VAO = subMesh.VAO;

				TODO( "Adding transforms like this might not be efficient, consider making the transform array reserve some space to avoid heap allocations?" );
				Matrix4 subMeshWorldTransform = a_Transform * subMesh.Transform;
				drawCommand.Transforms.emplace_back( std::move( subMeshWorldTransform ) );
			}
		}
	}
} // namespace Tridium