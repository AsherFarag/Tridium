#pragma once
#ifdef IS_EDITOR
#include <Tridium/Core/AssetManager.h>

namespace Tridium::Editor {

    class EditorAssetManager : public AssetManager
    {
    public:
        static SharedPtr<EditorAssetManager> Get() { return SharedPtrCast<EditorAssetManager>( s_Instance ); }

        template <typename T>
        static inline AssetRef<T> ImportAsset( const std::string& a_Path ) { return SharedPtrCast<T>( ImportAsset( a_Path ) ); }
        static AssetRef<Asset> ImportAsset( const std::string& a_Path );
    };

}

#endif // IS_EDITOR