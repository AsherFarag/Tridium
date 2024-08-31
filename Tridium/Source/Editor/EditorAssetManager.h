#pragma once
#ifdef IS_EDITOR
#include <Tridium/Asset/AssetManager.h>

namespace Tridium::Editor {

    class EditorAssetManager final : public AssetManager
    {
    public:
        static const EditorAssetManager* Get() { return static_cast<EditorAssetManager*>( s_Instance ); }

        static void ImportAsset( const IO::FilePath& a_Path, bool a_Override = false ) { Get()->Internal_ImportAsset( a_Path, a_Override ); }
        static bool SaveAsset( const IO::FilePath& a_Path, const AssetRef<Asset>& a_Asset ) { return Get()->Internal_SaveAsset( a_Path, a_Asset ); }

    protected:
        bool Internal_SaveAsset( const IO::FilePath a_Path, const AssetRef<Asset>& a_Asset );

        virtual bool HasAssetImpl( const AssetHandle& a_AssetHandle ) const override;
        virtual AssetRef<Asset> GetAssetImpl( const AssetHandle& a_AssetHandle, bool a_ShouldLoad ) override;
        virtual AssetRef<Asset> GetAssetImpl( const IO::FilePath& a_Path, bool a_ShouldLoad ) override;
        virtual AssetRef<Asset> LoadAssetImpl( const AssetHandle& a_AssetHandle ) override;
        virtual AssetRef<Asset> LoadAssetImpl( const IO::FilePath& a_Path ) override;
        virtual bool ReleaseAssetImpl( const AssetHandle& a_AssetHandle ) override;

        // ----------
        void Internal_ImportAsset( const IO::FilePath a_Path, bool a_Override );
        void ImportFBX( const IO::FilePath& a_Path );
        void ImportMesh( const IO::FilePath& a_Path );
        void ImportTexture( const IO::FilePath& a_Path );
        void ImportShader( const IO::FilePath& a_Path );
        void ImportMaterial( const IO::FilePath& a_Path );
        void ImportScene( const IO::FilePath& a_Path );

    private:
        std::unordered_map<AssetHandle, AssetRef<Asset>> m_Library;
		std::unordered_map<AssetHandle, IO::FilePath> m_Paths;
    };

    //class EditorAssetManager : public AssetManager
    //{
    //public:
    //    static SharedPtr<EditorAssetManager> Get() { return SharedPtrCast<EditorAssetManager>( s_Instance ); }
    // 
    //    static void ImportAsset( const IO::FilePath& a_Path, bool a_Override = false ) { Get()->Internal_ImportAsset( a_Path, a_Override ); }
    //    static bool SaveAsset( const IO::FilePath& a_Path, const AssetRef<Asset>& a_Asset ) { return Get()->Internal_SaveAsset( a_Path, a_Asset ); }

    //protected:
    //    virtual AssetRef<Asset> Internal_LoadAsset( const IO::FilePath& a_Path ) override;
    //    bool Internal_SaveAsset( const IO::FilePath a_Path, const AssetRef<Asset>& a_Asset );

    //    // ----------
    //    void Internal_ImportAsset( const IO::FilePath a_Path, bool a_Override );
    //    void ImportFBX( const IO::FilePath& a_Path );
    //    void ImportMesh( const IO::FilePath& a_Path );
    //    void ImportTexture( const IO::FilePath& a_Path );
    //    void ImportShader( const IO::FilePath& a_Path );
    //    void ImportMaterial( const IO::FilePath& a_Path );
    //    void ImportScene( const IO::FilePath& a_Path );
    //};
}

#endif // IS_EDITOR