#pragma once
#include "AssetManagerBase.h"
#include <Tridium/Engine/Engine.h>

namespace Tridium {

	// Static API for the Asset Manager.
	class AssetManager
	{
	public:
		static AssetManagerBase* Get() { return nullptr; }

		// Can return nullptr if the AssetManager is not of the correct type.
		// If with editor, this will return an instance of EditorAssetManager.
		template<typename T> requires Concepts::IsBaseOf<AssetManagerBase, T>
		static T* Get()
		{
			return Cast<T*>( Get() );
		}

		template<typename T> requires Concepts::IsBaseOf<Asset, T>
		static SharedPtr<T> GetAsset( OldAssetHandle a_Handle )
		{
			SharedPtr<Asset> asset = Get()->GetAsset( a_Handle );
			return SharedPtrCast<T>( asset );
		}

		template<typename T> requires Concepts::IsBaseOf<Asset, T>
		static SharedPtr<T> GetAsset( const FilePath& a_Path )
		{
			SharedPtr<Asset> asset = Get()->GetAsset( a_Path );
			return SharedPtrCast<T>( asset );
		}

		template<typename T> requires Concepts::IsBaseOf<Asset, T>
		static SharedPtr<T> GetMemoryOnlyAsset( OldAssetHandle a_Handle )
		{
			SharedPtr<Asset> asset = Get()->GetMemoryOnlyAsset( a_Handle );
			return SharedPtrCast<T>( asset );
		}

		template<typename T> requires Concepts::IsBaseOf<Asset, T>
		static bool AddMemoryOnlyAsset( OldAssetHandle a_Handle, SharedPtr<T> a_Asset ) 
		{
			return Get()->AddMemoryOnlyAsset( a_Handle, SharedPtrCast<Asset>( a_Asset ) );
		}

		template<typename T>
		static FilteredAssetStorageIterator<T> GetAssetsOfType() { return FilteredAssetStorageIterator<T>( Get()->GetAssets() ); }

		static AssetStorageIterator GetAssets() { return Get()->GetAssets(); }
		static bool HasAsset( OldAssetHandle a_Handle ) { return Get()->HasAsset( a_Handle ); }
		static void RemoveAsset( OldAssetHandle a_Handle ) { Get()->RemoveAsset( a_Handle ); }
		static EAssetTypeOld GetAssetType( OldAssetHandle a_Handle ) { return Get()->GetAssetType( a_Handle ); }
		static bool IsMemoryAsset( OldAssetHandle a_Handle ) { return Get()->IsMemoryAsset( a_Handle ); }
		static void RegisterDependency( OldAssetHandle a_Dependent, OldAssetHandle a_Dependency ) { Get()->RegisterDependency( a_Dependent, a_Dependency ); }
		static void UnregisterDependency( OldAssetHandle a_Dependent, OldAssetHandle a_Dependency ) { Get()->UnregisterDependency( a_Dependent, a_Dependency ); }

		static OldAssetHandle GetNextMemoryAssetHandle() { static OldAssetHandle::Type s_NextHandle = 0; return ++s_NextHandle; }
	};
}