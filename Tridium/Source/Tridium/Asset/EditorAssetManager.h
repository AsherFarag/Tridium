#pragma once
#ifdef IS_EDITOR
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Asset/AssetMetaData.h>

namespace Tridium::Editor {

	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetaData>;

	class EditorAssetManager final : public AssetManagerBase
	{
		template<typename T>
		using AssetStorageType = std::unordered_map<AssetHandle, T>;

	public:
		EditorAssetManager();
		virtual ~EditorAssetManager() override = default;

		// - Inherited via AssetManagerBase -
		void Init() override;
		void Shutdown() override;
		SharedPtr<Asset> GetAsset( AssetHandle a_Handle ) override;
		SharedPtr<Asset> GetMemoryOnlyAsset( AssetHandle a_Handle ) override;
		bool AddMemoryOnlyAsset( AssetHandle a_Handle, SharedPtr<Asset> a_Asset ) override;
		bool HasAsset( AssetHandle a_Handle ) override;
		void RemoveAsset( AssetHandle a_Handle ) override;
		EAssetType GetAssetType( AssetHandle a_Handle ) override;
		bool IsMemoryAsset( AssetHandle a_Handle ) override;
		void RegisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) override;
		void UnregisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) override;

		// - Editor -
		const AssetMetaData& GetAssetMetaData( AssetHandle a_Handle ) const;
		const AssetMetaData& GetAssetMetaData( const IO::FilePath& a_Path ) const;
		void SetAssetMetaData( const AssetMetaData& a_MetaData );

		AssetHandle ImportAsset( const IO::FilePath& a_Path );
		bool CreateAsset( const AssetMetaData& a_MetaData, SharedPtr<Asset> a_Asset );

	private:
		bool SerializeAssetRegistry();
		bool DeserializeAssetRegistry();

	private:
		AssetStorageType<SharedPtr<Asset>> m_LoadedAssets;
		AssetStorageType<SharedPtr<Asset>> m_MemoryAssets;
		AssetStorageType<std::unordered_set<AssetHandle>> m_AssetDependencies;
		AssetRegistry m_AssetRegistry;

		friend class AssetRegistryPanel;
	};

} // namespace Tridium::Editor

#endif // IS_EDITOR