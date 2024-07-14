#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Singleton.h>

#define ASSET_MANAGER_FILENAME "AssetManager.tmeta"

namespace Tridium {

#define ASSET_CLASS_TYPE(type) static EAssetType GetStaticType() { return EAssetType::type; }\
							   virtual EAssetType GetAssetType() { return GetStaticType(); }\
                               virtual const char* AssetTypeName() { return #type; }\

	enum class EAssetType
	{
		None = 0, Mesh, Shader, Texture, Material,
	};

	class Asset
	{
		friend class AssetManager;
	public:
		virtual EAssetType GetAssetType() { return EAssetType::None; }
		virtual const char* AssetTypeName() { return "None"; }

		const std::string& GetPath() const { return m_Path; }
		const GUID GetGUID() const { return m_GUID; }

		bool IsLoaded() const { return m_Loaded; }
		bool IsModified() const { return m_Modified; }
		void SetModified( bool modified ) { m_Modified = modified; }
		virtual bool Save() { return false; }

	protected:
		GUID m_GUID;
		std::string m_Path;
		bool m_Loaded = false;
		bool m_Modified = false;
	};

	class AssetManager final : public Singleton<AssetManager>
	{
		friend Singleton;
	public:
		auto& GetLibrary() { return m_Library; }

		/* Returns true if an asset with this GUID is loaded in the asset library. */
		template <typename T>
		static bool HasAsset( const GUID& assetGUID );

		// Returns the loaded asset with the corresponding GUID. 
		// If the asset isn't loaded and shouldLoad == true, the asset will be loaded and returned.
		// else, returns nullptr.
		template <typename T>
		static Ref<T> GetAsset( const GUID& assetGUID, bool shouldLoad = true );

		// Returns the loaded asset from the corresponding file path.
		// If the asset isn't loaded and shouldLoad == true, the asset will be loaded and returned.
		// else, returns nullptr.
		template <typename T>
		static Ref<T> GetAsset( const std::string& assetPath, bool shouldLoad = true );

		template <typename T>
		static Ref<T> LoadAsset( const GUID& assetGUID );
		template <typename T>
		static Ref<T> LoadAsset( const std::string& path );

		template <typename T>
		static bool AsyncLoadAsset( const GUID& assetGUID, Ref<T>& outAsset ) { return nullptr; }
		template <typename T>
		static bool AsyncLoadAsset( const std::string& path, Ref<T>& outAsset ) { return nullptr; }

		static void Serialize( const fs::path& path );
		bool Deserialize( const std::string& path );
		static Ref<Asset> Import( const fs::path& path );

	private:
		/* Deserializes the Asset Directory Meta File containing a map of GUID's to file paths and adds them to m_Paths. */
		virtual void Init() override;
		static void AddAsset( const Ref<Asset>& asset ) { Get().m_Library[asset->GetGUID()] = asset; }

	private:
		std::unordered_map<GUID, Ref<Asset>> m_Library;
		std::unordered_map<GUID, std::string> m_Paths;
	};

	template<typename T>
	bool AssetManager::HasAsset( const GUID& assetGUID )
	{
		if ( auto it = Get().m_Library.find( assetGUID ); it != Get().m_Library.end() )
		{
			if ( it->second->GetAssetType() != T::GetStaticType() )
			{
				return false;
			}

			return true;
		}

		return false;
	}

	template<typename T>
	inline Ref<T> AssetManager::GetAsset( const GUID& assetGUID, bool shouldLoad )
	{
		// Check if the asset is already loaded
		if ( auto it = Get().m_Library.find( assetGUID ); it != Get().m_Library.end() )
		{
			if ( it->second->GetAssetType() != T::GetStaticType() )
			{
				TE_CORE_WARN( "Attempted to get asset of a type, but the asset's type is different" );
				return nullptr;
			}

			return std::static_pointer_cast<T>( it->second );
		}

		if ( !shouldLoad )
			return nullptr;

		//Ref<T> asset = MakeRef<T>();
		//asset->m_GUID = assetGUID;
		//TODO( "Add asynchronous loading." );
		//if ( !LoadAsset( assetGUID, asset ) )
		//	return false;

		return LoadAsset<T>(assetGUID);
	}

	template<typename T>
	inline Ref<T> AssetManager::GetAsset( const std::string& assetPath, bool shouldLoad )
	{
		return Ref<T>();
	}

	template<typename T>
	inline Ref<T> AssetManager::LoadAsset( const GUID& assetGUID )
	{
		if ( auto it = Get().m_Paths.find( assetGUID ); it != Get().m_Paths.end() )
			return LoadAsset<T>( it->second );

		TE_CORE_WARN( "Could not find Asset Path for {0}", (GUID::Type)assetGUID.ID() );
		return nullptr;
	}

	template<typename T>
	inline Ref<T> AssetManager::LoadAsset( const std::string& path )
	{
		return T::Load( path );
	}
}