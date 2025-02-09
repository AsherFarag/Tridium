#pragma once
#if 1 //IS_EDITOR
#include "AssetManager.h"
#include "AssetRegistry.h"

namespace Tridium::Editor {

	class EditorAssetManager final : public AssetManagerBase
	{
	public:
		EditorAssetManager();
		virtual ~EditorAssetManager() override = default;

		// - Inherited via AssetManagerBase -
		void Init() override;
		void Shutdown() override;
		SharedPtr<Asset> GetAsset( AssetHandle a_Handle ) override;
		SharedPtr<Asset> GetAsset( const FilePath& a_Path ) override;
		SharedPtr<Asset> GetMemoryOnlyAsset( AssetHandle a_Handle ) override;
		AssetStorageIterator GetAssets() override;
		bool AddMemoryOnlyAsset( AssetHandle a_Handle, SharedPtr<Asset> a_Asset ) override;
		bool HasAsset( AssetHandle a_Handle ) override;
		void RemoveAsset( AssetHandle a_Handle ) override;
		EAssetType GetAssetType( AssetHandle a_Handle ) override;
		bool IsMemoryAsset( AssetHandle a_Handle ) override;
		void RegisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) override;
		void UnregisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) override;

		// - Editor -
		static auto Get() { return AssetManager::Get<EditorAssetManager>(); }

		const AssetMetaData& GetAssetMetaData( AssetHandle a_Handle ) const;
		const AssetMetaData& GetAssetMetaData( const FilePath& a_Path ) const;
		void SetAssetMetaData( const AssetMetaData& a_MetaData );

		const AssetStorageType& GetLoadedAssets() const { return m_LoadedAssets; }
		const AssetStorageType& GetMemoryAssets() const { return m_MemoryAssets; }
		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }

		bool SaveAsset( AssetHandle a_Handle );
		AssetHandle ImportAsset( const FilePath& a_Path );
		bool CreateAsset( const AssetMetaData& a_MetaData, SharedPtr<Asset> a_Asset );

		template<typename T>
		SharedPtr<T> CreateAsset( const FilePath& a_Path )
		{
			AssetMetaData metaData
			{
				.Handle = AssetHandle::Create(),
				.AssetType = T::StaticType(),
				.Path = a_Path,
				.Name = a_Path.GetFilenameWithoutExtension(),
				.IsAssetLoaded = true,
			};

			SharedPtr<T> asset = MakeShared<T>();
			if ( CreateAsset( metaData, SharedPtrCast<Asset>( asset ) ) )
			{
				return asset;
			}
			return nullptr;
		}

		// If the path is already absolute, it will be returned as is.
		// Otherwise, it will be appended to the project directory.
		FilePath GetAbsolutePath( const FilePath& a_Path ) const;

	private:
		bool SerializeAssetRegistry();
		bool DeserializeAssetRegistry();

	private:
		AssetStorageType m_LoadedAssets;
		AssetStorageType m_MemoryAssets;
		AssetRegistry m_AssetRegistry;

		friend class AssetRegistryPanel;
	};

} // namespace Tridium::Editor

#endif // IS_EDITOR