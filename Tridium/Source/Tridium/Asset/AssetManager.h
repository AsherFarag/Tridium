#pragma once
#include  "Asset.h"
#include <Tridium/Core/Application.h>

namespace Tridium {

	class AssetManagerBase
	{
	public:
		AssetManagerBase() = default;
		virtual ~AssetManagerBase() = default;

		virtual void Init() {};
		virtual void Shutdown() {};

		virtual SharedPtr<Asset> GetAsset( AssetHandle a_Handle ) = 0;
		virtual SharedPtr<Asset> GetAsset( const IO::FilePath& a_Path ) = 0;
		virtual SharedPtr<Asset> GetMemoryOnlyAsset( AssetHandle a_Handle ) = 0;
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
		static SharedPtr<AssetManagerBase> Get() { return Application::Get().m_AssetManager; }

		// Can return nullptr if the AssetManager is not of the correct type.
		// If macro IS_EDITOR is defined, this will return an instance of EditorAssetManager.
		template<typename T>
		static SharedPtr<T> Get()
		{
			static_assert( std::is_base_of_v<AssetManagerBase, T>, "T must inherit from AssetManagerBase" );
			return SharedPtrCast<T>( Application::Get().m_AssetManager );
		}

		template<typename T>
		static SharedPtr<T> GetAsset( AssetHandle a_Handle )
		{
			static_assert( std::is_base_of_v<Asset, T>, "T must inherit from Asset" );
			SharedPtr<Asset> asset = Application::Get().m_AssetManager->GetAsset( a_Handle );
			return SharedPtrCast<T>( asset );
		}

		template<typename T>
		static SharedPtr<T> GetAsset( const IO::FilePath& a_Path )
		{
			static_assert( std::is_base_of_v<Asset, T>, "T must inherit from Asset" );
			SharedPtr<Asset> asset = Application::Get().m_AssetManager->GetAsset( a_Path );
			return SharedPtrCast<T>( asset );
		}

		template<typename T>
		static SharedPtr<T> GetMemoryOnlyAsset( AssetHandle a_Handle )
		{
			static_assert( std::is_base_of_v<Asset, T>, "T must inherit from Asset" );
			SharedPtr<Asset> asset = Application::Get().m_AssetManager->GetMemoryOnlyAsset( a_Handle );
			return SharedPtrCast<T>( asset );
		}

		template<typename T>
		static bool AddMemoryOnlyAsset( AssetHandle a_Handle, SharedPtr<T> a_Asset ) 
		{
			static_assert( std::is_base_of_v<Asset, T>, "T must inherit from Asset" );
			return Application::Get().m_AssetManager->AddMemoryOnlyAsset( a_Handle, SharedPtrCast<Asset>( a_Asset ) );
		}

		static bool HasAsset( AssetHandle a_Handle ) { return Application::Get().m_AssetManager->HasAsset( a_Handle ); }
		static void RemoveAsset( AssetHandle a_Handle ) { Application::Get().m_AssetManager->RemoveAsset( a_Handle ); }
		static EAssetType GetAssetType( AssetHandle a_Handle ) { return Application::Get().m_AssetManager->GetAssetType( a_Handle ); }
		static bool IsMemoryAsset( AssetHandle a_Handle ) { return Application::Get().m_AssetManager->IsMemoryAsset( a_Handle ); }
		static void RegisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) { Application::Get().m_AssetManager->RegisterDependency( a_Dependent, a_Dependency ); }
		static void UnregisterDependency( AssetHandle a_Dependent, AssetHandle a_Dependency ) { Application::Get().m_AssetManager->UnregisterDependency( a_Dependent, a_Dependency ); }

		static AssetHandle GetNextMemoryAssetHandle() { static AssetHandle::Type s_NextHandle = 0; return ++s_NextHandle; }
	};
}