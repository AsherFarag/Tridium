#pragma once
#include <Tridium/Core/Core.h>

namespace Tridium {

	using AssetHandle = GUID;

	template <typename AssetHandle>
	class Asset
	{
	public:
		Asset() = default;
		virtual ~Asset() = default;

		inline const std::string& GetPath() const { return m_Path; }
		inline AssetHandle GetHandle() const { return m_Handle; }
		inline void _SetPath( const std::string& path ) { m_Path = path; }
		inline void _SetHandle( const AssetHandle& handle ) { m_Handle = handle; }

	protected:
		std::string m_Path;
		AssetHandle m_Handle;
	};

	template <typename T, typename AssetHandle, typename AssetType>
	class AssetLibrary
	{
	public:
		static T& Get();

	protected:
		static Ref<AssetType> GetAsset( const AssetHandle& handle );
		static bool GetHandle( const std::string& path, AssetHandle& outHandle );
		static AssetHandle GetHandle( const std::string& path );
		static bool HasHandle( const std::string& path );
		static bool AddAsset( const std::string& path, const Ref<AssetType>& asset );
		static bool RemoveAsset( const AssetHandle& handle );

	protected:
		std::unordered_map<std::string, AssetHandle> m_PathHandles;
		std::unordered_map<AssetHandle, Ref<AssetType>> m_Library;
	};

	template<typename T, typename AssetHandle, typename AssetType>
	inline T& AssetLibrary<T, AssetHandle, AssetType>::Get()
	{
		static T s_LibraryInstance;
		return s_LibraryInstance;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline Ref<AssetType> AssetLibrary<T, AssetHandle, AssetType>::GetAsset( const AssetHandle& handle )
	{
		if ( auto it = Get().m_Library.find( handle ); it != Get().m_Library.end() )
		{
			return it->second;
		}

		return nullptr;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline bool AssetLibrary<T, AssetHandle, AssetType>::GetHandle( const std::string& path, AssetHandle& outHandle )
	{
		if ( auto it = Get().m_PathHandles.find( path ); it != Get().m_PathHandles.end() )
		{
			outHandle = it->second;
			return true;
		}

		return false;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline AssetHandle AssetLibrary<T, AssetHandle, AssetType>::GetHandle( const std::string& path )
	{
		if ( auto it = Get().m_PathHandles.find( path ); it != Get().m_PathHandles.end() )
		{
			return it->second;
		}

		return {};
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline bool AssetLibrary<T, AssetHandle, AssetType>::HasHandle( const std::string& path )
	{
		if ( auto it = Get().m_PathHandles.find( path ); it != Get().m_PathHandles.end() )
		{
			return true;
		}

		return false;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline bool AssetLibrary<T, AssetHandle, AssetType>::AddAsset( const std::string& path, const Ref<AssetType>& asset )
	{
		TE_CORE_ASSERT( asset->GetHandle().Valid() );
		if ( HasHandle( path ) )
			return false;

		if ( GetAsset( asset->GetHandle() ) )
			return false;

		Get().m_PathHandles.emplace( path, asset->GetHandle() );
		Get().m_Library.emplace( asset->GetHandle(), asset );

		return true;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline bool AssetLibrary<T, AssetHandle, AssetType>::RemoveAsset( const AssetHandle& handle )
	{
		TE_CORE_ASSERT( handle.Valid() );

		for ( auto it = Get().m_PathHandles.begin(); it != Get().m_PathHandles.end(); ++it )
		{
			if ( it->second == handle )
			{
				Get().m_PathHandles.erase( it );
				break;
			}
		}

		if ( auto it = Get().m_Library.find( handle ); it != Get().m_Library.end() )
		{
			Get().m_Library.erase( it );
			return true;
		}

		return false;
	}

}