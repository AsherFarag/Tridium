#pragma once
#ifdef IS_EDITOR
#include <Tridium/Asset/AssetManager.h>



namespace Tridium {

    struct AssetMetaData;

    namespace Editor {

        class EditorAssetManager final : public AssetManager
        {
        public:
            static EditorAssetManager* Get() { return static_cast<EditorAssetManager*>( s_Instance ); }

            static AssetRef<Asset> ImportAsset( const IO::FilePath& a_Path, bool a_Override = false ) { return Get()->ImportAssetImpl( a_Path, a_Override ); }
            static bool SaveAsset( const IO::FilePath& a_Path, const AssetRef<Asset>& a_Asset ) { return Get()->SaveAssetImpl( a_Path, a_Asset ); }
            static bool AddAsset( const AssetRef<Asset>& a_Asset ) { return Get()->AddAssetImpl( a_Asset ); }
            static AssetHandle GetAssetHandle( const IO::FilePath& a_Path ) { return Get()->GetAssetHandleImpl( a_Path ); }

            /*
                a_Path must include the MetaData file extension. 
                E.g. Path/To/Asset.type.tmeta
            */
            static AssetMetaData* LoadAssetMetaData( const IO::FilePath& a_Path );

            template<typename T>
            static T* LoadAssetMetaData( const IO::FilePath& a_Path ) { return static_cast<T*>( LoadAssetMetaDataImpl( a_Path ) ); }

        protected:
            static void ApplyMetaDataToAsset( Asset& a_Asset, const AssetMetaData& a_MetaData );

            AssetRef<Asset> ImportAssetImpl( const IO::FilePath& a_Path, bool a_Override = false );
            bool SaveAssetImpl( const IO::FilePath& a_Path, const AssetRef<Asset>& a_Asset );
            bool AddAssetImpl( const AssetRef<Asset>& a_Asset );
            AssetHandle GetAssetHandleImpl( const IO::FilePath& a_Path ) const;
            AssetMetaData* LoadAssetMetaDataImpl( const IO::FilePath& a_Path ) const;

            virtual bool HasAssetImpl( const AssetHandle& a_AssetHandle ) const override;
            virtual AssetRef<Asset> GetAssetImpl( const AssetHandle& a_AssetHandle ) override;
            virtual AssetRef<Asset> GetAssetImpl( const IO::FilePath& a_Path ) override;
            virtual AssetRef<Asset> LoadAssetImpl( const AssetHandle& a_AssetHandle ) override;
            virtual AssetRef<Asset> LoadAssetImpl( const IO::FilePath& a_Path ) override;
            virtual bool ReleaseAssetImpl( const AssetHandle& a_AssetHandle ) override;

        private:
            std::unordered_map<AssetHandle, AssetRef<Asset>> m_Library;
            std::unordered_map<AssetHandle, IO::FilePath> m_Paths;
            std::unordered_map<AssetHandle, std::unordered_set<AssetHandle>> m_Dependencies;
        };

    }
}

#endif // IS_EDITOR