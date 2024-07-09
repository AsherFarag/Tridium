#pragma once
#include <Tridium/Core/Core.h>

namespace Tridium {

	using AssetHandle = GUID;

	class Asset
	{
	public:
		Asset() = default;
		virtual ~Asset() = default;

		bool IsModified() const { return m_Modified; }
		void SetModified( bool modified );

		const std::string& GetPath() const { return m_Path; }
		const AssetHandle GetHandle() const { return m_Handle; }
		void _SetPath( const std::string& path ) { m_Path = path; }
		void _SetHandle( const AssetHandle& handle ) { m_Handle = handle; }

	protected:
		std::string m_Path;
		GUID m_Handle;
		bool m_Modified = false;
	};

	template <typename T, typename AssetHandle, typename AssetType>
	class AssetLibrary
	{
	public:
		static T& Get();

		auto& GetLibrary() { return m_Library; }

	protected:
		virtual void Init() {};

		Ref<AssetType> GetAsset( const AssetHandle& handle );
		bool GetHandle( const std::string& path, AssetHandle& outHandle );
		AssetHandle GetHandle( const std::string& path );
		bool HasHandle( const std::string& path );
		bool AddAsset( const std::string& path, const Ref<AssetType>& asset );
		bool RemoveAsset( const AssetHandle& handle );

	protected:
		std::unordered_map<std::string, AssetHandle> m_PathHandles;
		std::unordered_map<AssetHandle, Ref<AssetType>> m_Library;
	};

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
	inline Ref<AssetType> AssetLibrary<T, AssetHandle, AssetType>::GetAsset( const AssetHandle& handle )
	{
		if ( auto it = m_Library.find( handle ); it != m_Library.end() )
		{
			return it->second;
		}

		return nullptr;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline bool AssetLibrary<T, AssetHandle, AssetType>::GetHandle( const std::string& path, AssetHandle& outHandle )
	{
		if ( auto it = m_PathHandles.find( path ); it != m_PathHandles.end() )
		{
			outHandle = it->second;
			return true;
		}

		return false;
	}

	template<typename T, typename AssetHandle, typename AssetType>
	inline AssetHandle AssetLibrary<T, AssetHandle, AssetType>::GetHandle( const std::string& path )
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
		TE_CORE_ASSERT( asset->GetHandle().Valid() );
		if ( HasHandle( path ) )
			return false;

		if ( GetAsset( asset->GetHandle() ) )
			return false;

		m_PathHandles.emplace( path, asset->GetHandle() );
		m_Library.emplace( asset->GetHandle(), asset );

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