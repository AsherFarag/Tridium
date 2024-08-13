#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorAssetManager.h"

namespace Tridium::Editor {

    AssetRef<Asset> EditorAssetManager::ImportAsset( const std::string& a_Path )
    {
        return AssetRef<Asset>();
    }

}

#endif