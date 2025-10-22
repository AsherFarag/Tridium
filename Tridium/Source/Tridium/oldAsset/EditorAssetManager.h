#pragma once
#if 1 //IS_EDITOR
#include "AssetManager.h"
#include "AssetRegistry.h"

namespace Tridium {

	class EditorAssetManager final : public AssetManagerBase
	{
	public:
		EditorAssetManager();
		virtual ~EditorAssetManager() override = default;

		// - Inherited via AssetManagerBase -
		void Init() override;
		void Shutdown() override;
		SharedPtr<Asset> GetAsset( OldAssetHandle a_Handle ) override;
		SharedPtr<Asset> GetAsset( const FilePath& a_Path ) override;
		SharedPtr<Asset> GetMemoryOnlyAsset( OldAssetHandle a_Handle ) override;
		AssetStorageIterator GetAssets() override;
		bool AddMemoryOnlyAsset( OldAssetHandle a_Handle, SharedPtr<Asset> a_Asset ) override;
		bool HasAsset( OldAssetHandle a_Handle ) override;
		void RemoveAsset( OldAssetHandle a_Handle ) override;
		EAssetTypeOld GetAssetType( OldAssetHandle a_Handle ) override;
		bool IsMemoryAsset( OldAssetHandle a_Handle ) override;
		void RegisterDependency( OldAssetHandle a_Dependent, OldAssetHandle a_Dependency ) override;
		void UnregisterDependency( OldAssetHandle a_Dependent, OldAssetHandle a_Dependency ) override;

		// - Editor -
		static auto Get() { return AssetManager::Get<EditorAssetManager>(); }

		const OldAssetMetaData& GetAssetMetaData( OldAssetHandle a_Handle ) const;
		const OldAssetMetaData& GetAssetMetaData( const FilePath& a_Path ) const;
		void SetAssetMetaData( const OldAssetMetaData& a_MetaData );

		const AssetStorageType& GetLoadedAssets() const { return m_LoadedAssets; }
		const AssetStorageType& GetMemoryAssets() const { return m_MemoryAssets; }
		const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }

		bool SaveAsset( OldAssetHandle a_Handle );
		OldAssetHandle ImportAsset( const FilePath& a_Path );
		bool CreateAsset( const OldAssetMetaData& a_MetaData, SharedPtr<Asset> a_Asset );

		template<typename T>
		SharedPtr<T> CreateAsset( const FilePath& a_Path )
		{
			OldAssetMetaData metaData
			{
				.Handle = OldAssetHandle::Create(),
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

} // namespace Tridium

#endif // IS_EDITOR