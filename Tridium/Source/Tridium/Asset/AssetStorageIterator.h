#pragma once

namespace Tridium {

	template<typename T>
	class FilteredAssetStorageIterator;

	// The Asset Iterator List iterates over all assets in the AssetManager including memory only assets.
	class AssetStorageIterator
	{
	public:
		AssetStorageIterator( const AssetStorageType& a_LoadedAssets, const AssetStorageType& a_MemoryAssets )
			: m_LoadedAssets( a_LoadedAssets )
			, m_MemoryAssets( a_MemoryAssets )
		{
		}

		// Iterator for all assets in the AssetManager.
		class Iterator
		{
		public:
			Iterator( const AssetStorageType& a_LoadedAssets, const AssetStorageType& a_MemoryAssets )
				: m_LoadedAssets( a_LoadedAssets )
				, m_MemoryAssets( a_MemoryAssets )
			{
				m_LoadedIterator = m_LoadedAssets.begin();
				m_MemoryIterator = m_MemoryAssets.begin();
			}

			SharedPtr<Asset> operator*() const
			{
				if ( m_LoadedIterator != m_LoadedAssets.end() )
				{
					return m_LoadedIterator->second;
				}
				else if ( m_MemoryIterator != m_MemoryAssets.end() )
				{
					return m_MemoryIterator->second;
				}
				return nullptr;
			}

			Iterator& operator++()
			{
				if ( m_LoadedIterator != m_LoadedAssets.end() )
				{
					++m_LoadedIterator;
				}
				else if ( m_MemoryIterator != m_MemoryAssets.end() )
				{
					++m_MemoryIterator;
				}
				return *this;
			}

			bool operator!=( const Iterator& a_Other ) const
			{
				return m_LoadedIterator != a_Other.m_LoadedIterator || m_MemoryIterator != a_Other.m_MemoryIterator;
			}

		private:
			const AssetStorageType& m_LoadedAssets;
			const AssetStorageType& m_MemoryAssets;
			AssetStorageType::const_iterator m_LoadedIterator;
			AssetStorageType::const_iterator m_MemoryIterator;
		};

		Iterator begin() const
		{
			return Iterator( m_LoadedAssets, m_MemoryAssets );
		}

		Iterator end() const
		{
			return Iterator( m_LoadedAssets, m_MemoryAssets );
		}

	private:
		const AssetStorageType& m_LoadedAssets;
		const AssetStorageType& m_MemoryAssets;

		template<typename T>
		friend class FilteredAssetStorageIterator;
	};

	template<typename T>
	class FilteredAssetStorageIterator
	{
	public:
		FilteredAssetStorageIterator( const AssetStorageType& a_LoadedAssets, const AssetStorageType& a_MemoryAssets )
			: m_LoadedAssets( a_LoadedAssets )
			, m_MemoryAssets( a_MemoryAssets )
		{
		}

		FilteredAssetStorageIterator( const AssetStorageIterator& a_AssetStorageIterator )
			: m_LoadedAssets( a_AssetStorageIterator.m_LoadedAssets )
			, m_MemoryAssets( a_AssetStorageIterator.m_MemoryAssets )
		{
		}

		// Iterator for all assets in the AssetManager.
		class Iterator
		{
		public:
			Iterator( const AssetStorageType& a_LoadedAssets, const AssetStorageType& a_MemoryAssets )
				: m_LoadedAssets( a_LoadedAssets )
				, m_MemoryAssets( a_MemoryAssets )
			{
				m_LoadedIterator = m_LoadedAssets.begin();
				m_MemoryIterator = m_MemoryAssets.begin();
			}

			SharedPtr<T> operator*() const
			{
				if ( m_LoadedIterator != m_LoadedAssets.end() )
				{
					return SharedPtrCast<T>( m_LoadedIterator->second );
				}
				else if ( m_MemoryIterator != m_MemoryAssets.end() )
				{
					return SharedPtrCast<T>( m_MemoryIterator->second );
				}
				return nullptr;
			}

			Iterator& operator++()
			{
				while ( m_LoadedIterator != m_LoadedAssets.end() )
				{
					if ( m_LoadedIterator->second->AssetType() == T::StaticType() )
					{
						++m_LoadedIterator;
						return *this;
					}
					++m_LoadedIterator;
				}

				while ( m_MemoryIterator != m_MemoryAssets.end() )
				{
					if ( m_MemoryIterator->second->AssetType() == T::StaticType() )
					{
						++m_MemoryIterator;
						return *this;
					}
					++m_MemoryIterator;
				}

				return *this;
			}

			bool operator!=( const Iterator& a_Other ) const
			{
				return m_LoadedIterator != a_Other.m_LoadedIterator || m_MemoryIterator != a_Other.m_MemoryIterator;
			}

			bool operator==( const Iterator& a_Other ) const
			{
				return m_LoadedIterator == a_Other.m_LoadedIterator && m_MemoryIterator == a_Other.m_MemoryIterator;
			}

		private:
			const AssetStorageType& m_LoadedAssets;
			const AssetStorageType& m_MemoryAssets;
			AssetStorageType::const_iterator m_LoadedIterator;
			AssetStorageType::const_iterator m_MemoryIterator;
		};

		Iterator begin() const
		{
			return Iterator( m_LoadedAssets, m_MemoryAssets );
		}

		Iterator end() const
		{
			return Iterator( m_LoadedAssets, m_MemoryAssets );
		}

	private:
		const AssetStorageType& m_LoadedAssets;
		const AssetStorageType& m_MemoryAssets;
	};
}