#pragma once
#ifdef IS_EDITOR
#include <Tridium/Asset/AssetManager.h>

namespace Tridium::Editor {

    class EditorAssetManager final : public AssetManager
    {
    public:
        static EditorAssetManager* Get() { return static_cast<EditorAssetManager*>( s_Instance ); }

        static void ImportAsset( const IO::FilePath& a_Path, bool a_Override = false ) { Get()->ImportAssetImpl( a_Path, a_Override ); }
        static bool SaveAsset( const IO::FilePath& a_Path, const AssetRef<Asset>& a_Asset ) { return Get()->SaveAssetImpl( a_Path, a_Asset ); }
        static bool AddAsset( const AssetRef<Asset>& a_Asset ) { return Get()->AddAssetImpl( a_Asset ); }

    protected:
        void ImportAssetImpl( const IO::FilePath& a_Path, bool a_Override = false );
        bool SaveAssetImpl( const IO::FilePath& a_Path, const AssetRef<Asset>& a_Asset );
        bool AddAssetImpl( const AssetRef<Asset>& a_Asset );

        virtual bool HasAssetImpl( const AssetHandle& a_AssetHandle ) const override;
        virtual AssetRef<Asset> GetAssetImpl( const AssetHandle& a_AssetHandle, bool a_ShouldLoad ) override;
        virtual AssetRef<Asset> GetAssetImpl( const IO::FilePath& a_Path, bool a_ShouldLoad ) override;
        virtual AssetRef<Asset> LoadAssetImpl( const AssetHandle& a_AssetHandle ) override;
        virtual AssetRef<Asset> LoadAssetImpl( const IO::FilePath& a_Path ) override;
        virtual bool ReleaseAssetImpl( const AssetHandle& a_AssetHandle ) override;

        // ----------
        void ImportFBX( const IO::FilePath& a_Path );
        void ImportMesh( const IO::FilePath& a_Path );
        void ImportTexture( const IO::FilePath& a_Path );
        void ImportShader( const IO::FilePath& a_Path );
        void ImportMaterial( const IO::FilePath& a_Path );
        void ImportScene( const IO::FilePath& a_Path );

    private:
        std::unordered_map<AssetHandle, AssetRef<Asset>> m_Library;
		std::unordered_map<AssetHandle, IO::FilePath> m_Paths;
        std::unordered_map<AssetHandle, std::unordered_set<AssetHandle>> m_Dependencies;
    };

}

#endif // IS_EDITOR