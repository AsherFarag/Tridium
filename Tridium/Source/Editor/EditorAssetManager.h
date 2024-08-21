#pragma once
#ifdef IS_EDITOR
#include <Tridium/Asset/AssetManager.h>

namespace Tridium::Editor {

    class EditorAssetManager : public AssetManager
    {
    public:
        static SharedPtr<EditorAssetManager> Get() { return SharedPtrCast<EditorAssetManager>( s_Instance ); }
     
        static void ImportAsset( const IO::FilePath& a_Path ) { Get()->Internal_ImportAsset( a_Path ); }

    protected:
        void Internal_ImportAsset( const IO::FilePath a_Path );
        void Internal_ImportFBX( const IO::FilePath& a_Path );
        virtual AssetRef<Asset> Internal_LoadAsset( const IO::FilePath& a_Path ) override;
    };
}

#endif // IS_EDITOR