#pragma once
#ifdef IS_EDITOR
#include <Tridium/Asset/AssetManager.h>

namespace Tridium::Editor {

    class EditorAssetManager : public AssetManager
    {
    public:
        static SharedPtr<EditorAssetManager> Get() { return SharedPtrCast<EditorAssetManager>( s_Instance ); }
     
        static void ImportAsset( const IO::FilePath& a_Path, bool a_Override = false ) { Get()->Internal_ImportAsset( a_Path, a_Override ); }

    protected:
        virtual AssetRef<Asset> Internal_LoadAsset( const IO::FilePath& a_Path ) override;

        // ----------
        void Internal_ImportAsset( const IO::FilePath a_Path, bool a_Override );
        void ImportFBX( const IO::FilePath& a_Path );
        void ImportMesh( const IO::FilePath& a_Path );
        void ImportTexture( const IO::FilePath& a_Path );
        void ImportShader( const IO::FilePath& a_Path );
        void ImportMaterial( const IO::FilePath& a_Path );
        void ImportScene( const IO::FilePath& a_Path );
    };
}

#endif // IS_EDITOR