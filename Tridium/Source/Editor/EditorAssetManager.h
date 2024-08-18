#pragma once
#ifdef IS_EDITOR
#include <Tridium/Asset/AssetManager.h>

namespace Tridium::Editor {

    class EditorAssetManager : public AssetManager
    {
    public:
        static SharedPtr<EditorAssetManager> Get() { return SharedPtrCast<EditorAssetManager>( s_Instance ); }

        template <typename T>
        static inline AssetRef<T> ImportAsset( const std::string& a_Path ) { return ImportAsset( a_Path ).As<T>(); }
        static AssetRef<Asset> ImportAsset( const std::string& a_Path );
    };

}

#endif // IS_EDITOR