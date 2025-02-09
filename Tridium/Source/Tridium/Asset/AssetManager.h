#pragma once
#include "Asset.h"
#include <Tridium/IO/FilePath.h>
#include <Tridium/Engine/Engine.h>
#include <Tridium/Utils/Concepts.h>

namespace Tridium {
	using AssetStorageType = std::unordered_map<AssetHandle, SharedPtr<Asset>>;
}

#include "AssetStorageIterator.h"

namespace Tridium {

	class AssetManagerBase
	{
	public:
		AssetManagerBase() = default;
		virtual ~AssetManagerBase() = default;

		virtual void Init() {};
		virtual void Shutdown() {};

		virtual SharedPtr<Asset> GetAsset( AssetHandle a_Handle ) = 0;
		virtual SharedPtr<Asset> GetAsset( const FilePath& a_Path ) = 0;
		virtual SharedPtr<Asset> GetMemoryOnlyAsset( AssetHandle a_Handle ) = 0;
		virtual AssetStorageIterator GetAssets() = 0;
		virtual bool AddMemoryOnlyAsset( AssetHandle a_Handle, SharedPtr<Asset> a_Asset ) = 0;
		virtual bool HasAsset( AssetHandle a_Handle ) = 0;
		virtual void RemoveAsset( AssetHandle a_Handle ) = 0;
		virtual EAssetType GetAssetType( AssetHandle a_Handle ) = 0;
		virtual bool IsMemoryAsset( AssetHandle a_Handle ) = 0;
		virtual void RegisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) = 0;
		virtual void UnregisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) = 0;
	};

	// Static API for the Asset Manager.
	class AssetManager
	{
	public:
		static AssetManagerBase* Get() { return Engine::Get()->GetAssetManager(); }

		// Can return nullptr if the AssetManager is not of the correct type.
		// If with editor, this will return an instance of EditorAssetManager.
		template<typename T> requires Concepts::IsBaseOf<AssetManagerBase, T>
		static T* Get()
		{
			return static_cast<T*>( Engine::Get()->GetAssetManager() );
		}

		template<typename T> requires Concepts::IsBaseOf<Asset, T>
		static SharedPtr<T> GetAsset( AssetHandle a_Handle )
		{
			SharedPtr<Asset> asset = Engine::Get()->GetAssetManager()->GetAsset( a_Handle );
			return SharedPtrCast<T>( asset );
		}

		template<typename T> requires Concepts::IsBaseOf<Asset, T>
		static SharedPtr<T> GetAsset( const FilePath& a_Path )
		{
			SharedPtr<Asset> asset = Engine::Get()->GetAssetManager()->GetAsset( a_Path );
			return SharedPtrCast<T>( asset );
		}

		template<typename T> requires Concepts::IsBaseOf<Asset, T>
		static SharedPtr<T> GetMemoryOnlyAsset( AssetHandle a_Handle )
		{
			SharedPtr<Asset> asset = Engine::Get()->GetAssetManager()->GetMemoryOnlyAsset( a_Handle );
			return SharedPtrCast<T>( asset );
		}

		template<typename T> requires Concepts::IsBaseOf<Asset, T>
		static bool AddMemoryOnlyAsset( AssetHandle a_Handle, SharedPtr<T> a_Asset ) 
		{
			return Engine::Get()->GetAssetManager()->AddMemoryOnlyAsset( a_Handle, SharedPtrCast<Asset>( a_Asset ) );
		}

		template<typename T>
		static FilteredAssetStorageIterator<T> GetAssetsOfType() { return FilteredAssetStorageIterator<T>( Engine::Get()->GetAssetManager()->GetAssets() ); }

		static AssetStorageIterator GetAssets() { return Engine::Get()->GetAssetManager()->GetAssets(); }
		static bool HasAsset( AssetHandle a_Handle ) { return Engine::Get()->GetAssetManager()->HasAsset( a_Handle ); }
		static void RemoveAsset( AssetHandle a_Handle ) { Engine::Get()->GetAssetManager()->RemoveAsset( a_Handle ); }
		static EAssetType GetAssetType( AssetHandle a_Handle ) { return Engine::Get()->GetAssetManager()->GetAssetType( a_Handle ); }
		static bool IsMemoryAsset( AssetHandle a_Handle ) { return Engine::Get()->GetAssetManager()->IsMemoryAsset( a_Handle ); }
		static void RegisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) { Engine::Get()->GetAssetManager()->RegisterDependency( a_Dependent, a_Dependency ); }
		static void UnregisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) { Engine::Get()->GetAssetManager()->UnregisterDependency( a_Dependent, a_Dependency ); }

		static AssetHandle GetNextMemoryAssetHandle() { static AssetHandle::Type s_NextHandle = 0; return ++s_NextHandle; }
	};
}