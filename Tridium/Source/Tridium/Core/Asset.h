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

	protected:
		GUID m_GUID;
		std::string m_Path;
		bool m_Loaded = false;
		bool m_Modified = false;
	};

	class AssetManager final : public Singleton<AssetManager>
	{
		friend class Singleton<AssetManager>;
	public:
		auto& GetLibrary() { return m_Library; }

		/* Returns true if an asset with this GUID is loaded in the asset library. */
		template <typename T>
		static bool HasAsset( const GUID& assetGUID );
		/*
		Returns the loaded asset with the corresponding GUID. 
		If the asset isn't loaded and shouldLoad == true, the asset will be loaded and returned.
		else, returns nullptr.
		*/
		template <typename T>
		static Ref<T> GetAsset( const GUID& assetGUID, bool shouldLoad = true );

		template <typename T>
		static bool LoadAsset( const GUID& assetGUID, Ref<T>& outAsset );
		template <typename T>
		static bool LoadAsset( const std::string& path, Ref<T>& outAsset );

		static void Serialize( const fs::path& path );
		bool Deserialize( const std::string& path );
		static Ref<Asset> Import( const fs::path& path );

	private:
		AssetManager() = default;
		/* Deserializes the Asset Directory Meta File containing a map of GUID's to file paths and adds them to m_Paths. */
		virtual void Init() override;
		static void AddAsset( const Ref<Asset>& asset ) { Get().m_Library[asset->GetGUID()] = asset; }

	private:
		std::unordered_map<GUID, Ref<Asset>> m_Library;
		std::unordered_map<GUID, std::string> m_Paths;
	};
}