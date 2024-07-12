#pragma once
#include <Tridium/Core/Core.h>

namespace Tridium {

	using AssetHandle = GUID;

	struct AssetMetaData
	{
		std::string Path;
		GUID ID;
		bool Modified = false;
	};

	template <typename T>
	class Asset
	{
	public:
		static Ref<T> Create();

		bool IsModified() const { return m_MetaData.Modified; }
		virtual void SetModified( bool modified ) { m_MetaData.Modified = modified; }

		const std::string& GetPath() const { return m_MetaData.Path; }
		const GUID GetGUID() const { return m_MetaData.ID; }
		void _SetPath( const std::string& path ) { m_MetaData.Path = path; }

	protected:
		AssetMetaData m_MetaData;
	};

	class AssetLibrary final
	{
	public:
		static AssetLibrary& Get();
		static AssetHandle Create( const std::string& path );

		auto& GetLibrary() { return m_Library; }

	protected:
		virtual void Init() {};

		Ref<AssetType> GetAsset( const AssetHandle& handle );
		bool GetGUID( const std::string& path, AssetHandle& outHandle );
		AssetHandle GetGUID( const std::string& path );
		bool HasHandle( const std::string& path );
		bool AddAsset( const std::string& path, const Ref<AssetType>& asset );
		bool RemoveAsset( const AssetHandle& handle );

	protected:
		std::unordered_map<std::string, AssetHandle> m_PathHandles;
		std::unordered_map<AssetHandle, Ref<AssetType>> m_Library;
	};

	//template <typename T, typename AssetHandle, typename AssetType>
	//class AssetLibrary
	//{
	//public:
	//	static T& Get();
	//	static AssetHandle Create( const std::string& path );

	//	auto& GetLibrary() { return m_Library; }

	//protected:
	//	virtual void Init() {};

	//	Ref<AssetType> GetAsset( const AssetHandle& handle );
	//	bool GetGUID( const std::string& path, AssetHandle& outHandle );
	//	AssetHandle GetGUID( const std::string& path );
	//	bool HasHandle( const std::string& path );
	//	bool AddAsset( const std::string& path, const Ref<AssetType>& asset );
	//	bool RemoveAsset( const AssetHandle& handle );

	//protected:
	//	std::unordered_map<std::string, AssetHandle> m_PathHandles;
	//	std::unordered_map<AssetHandle, Ref<AssetType>> m_Library;
	//};

	template<typename T, typename AssetHandle, typename AssetType>
	inline T& AssetLibrary<T, AssetHandle, AssetType>::Get()
	{
		static T s_LibraryInstance;
		static bool m_HasBeenInitialised = false;
		if ( !m_HasBeenInitialised )
		{
			m_HasBeenInitialised = true;
			s_LibraryInstance.Init();
		}

		return s_LibraryInstance;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline AssetHandle AssetLibrary<T, AssetHandle, AssetType>::Create( const std::string& path )
	{
		if ( auto handle = Get().GetGUID( path ); handle.Valid() )
			return handle;

		auto asset = MakeRef<AssetType>();
		asset->_SetHandle( AssetHandle::Create() );
		asset->_SetPath( path );
		Get().AddAsset( path, asset );

		return asset->GetGUID();
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline Ref<AssetType> AssetLibrary<T, AssetHandle, AssetType>::GetAsset( const AssetHandle& handle )
	{
		if ( auto it = m_Library.find( handle ); it != m_Library.end() )
		{
			return it->second;
		}

		return nullptr;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline bool AssetLibrary<T, AssetHandle, AssetType>::GetGUID( const std::string& path, AssetHandle& outHandle )
	{
		if ( auto it = m_PathHandles.find( path ); it != m_PathHandles.end() )
		{
			outHandle = it->second;
			return true;
		}

		return false;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline AssetHandle AssetLibrary<T, AssetHandle, AssetType>::GetGUID( const std::string& path )
	{
		if ( auto it = m_PathHandles.find( path ); it != m_PathHandles.end() )
		{
			return it->second;
		}

		return {};
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline bool AssetLibrary<T, AssetHandle, AssetType>::HasHandle( const std::string& path )
	{
		if ( auto it = m_PathHandles.find( path ); it != m_PathHandles.end() )
		{
			return true;
		}

		return false;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline bool AssetLibrary<T, AssetHandle, AssetType>::AddAsset( const std::string& path, const Ref<AssetType>& asset )
	{
		TE_CORE_ASSERT( asset->GetGUID().Valid() );
		if ( HasHandle( path ) )
			return false;

		if ( GetAsset( asset->GetGUID() ) )
			return false;

		m_PathHandles.emplace( path, asset->GetGUID() );
		m_Library.emplace( asset->GetGUID(), asset );

		return true;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline bool AssetLibrary<T, AssetHandle, AssetType>::RemoveAsset( const AssetHandle& handle )
	{
		TE_CORE_ASSERT( handle.Valid() );

		for ( auto it = m_PathHandles.begin(); it != m_PathHandles.end(); ++it )
		{
			if ( it->second == handle )
			{
				m_PathHandles.erase( it );
				break;
			}
		}

		if ( auto it = m_Library.find( handle ); it != m_Library.end() )
		{
			m_Library.erase( it );
			return true;
		}

		return false;
	}

}