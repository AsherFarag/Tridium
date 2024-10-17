#include "tripch.h"
#include "MeshLoader.h"

#include <Tridium/Rendering/Buffer.h>
#include <Tridium/Rendering/VertexArray.h>

#include "AssimpImporter.h"

namespace Tridium {

    SharedPtr<Asset> MeshSourceLoader::LoadAsset( const AssetMetaData& a_MetaData )
    {
        AssimpImporter importer( a_MetaData.Path );
		SharedPtr<MeshSource> meshSource = importer.ImportMeshSource();
        if ( !meshSource )
            return nullptr;

		meshSource->SetHandle( a_MetaData.Handle );
        return SharedPtrCast<Asset>( meshSource );
    }

    void StaticMeshLoader::SaveAsset( const AssetMetaData& a_MetaData, const SharedPtr<Asset>& a_Asset )
    {
    }

    SharedPtr<Asset> StaticMeshLoader::LoadAsset( const AssetMetaData& a_MetaData )
    {
        return SharedPtr<Asset>();
    }
}