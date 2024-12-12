#pragma once

namespace Tridium {

    template <typename T>
    class FilteredAssetStorageIterator;

    // The AssetStorageIterator iterates over all assets in the AssetManager, including in-memory-only assets.
    class AssetStorageIterator {
    public:
        AssetStorageIterator( const AssetStorageType& a_LoadedAssets, const AssetStorageType& a_MemoryAssets )
            : m_LoadedAssets( a_LoadedAssets ), m_MemoryAssets( a_MemoryAssets ) {}

        // Iterator for all assets in the AssetManager.
        class Iterator {
        public:
            Iterator( const AssetStorageType& a_LoadedAssets, const AssetStorageType& a_MemoryAssets,
                AssetStorageType::const_iterator a_LoadedIt, AssetStorageType::const_iterator a_MemoryIt )
                : m_LoadedAssets( a_LoadedAssets ), m_MemoryAssets( a_MemoryAssets ),
                m_LoadedIterator( a_LoadedIt ), m_MemoryIterator( a_MemoryIt ) {}

            SharedPtr<Asset> operator*() const 
            {
                if ( m_LoadedIterator != m_LoadedAssets.end() )
                    return m_LoadedIterator->second;
                if ( m_MemoryIterator != m_MemoryAssets.end() )
                    return m_MemoryIterator->second;

                return nullptr;
            }

            Iterator& operator++() 
            {
                if ( m_LoadedIterator != m_LoadedAssets.end() )
                    ++m_LoadedIterator;
                else if ( m_MemoryIterator != m_MemoryAssets.end() )
                    ++m_MemoryIterator;

                return *this;
            }

            bool operator!=( const Iterator& other ) const 
            {
                return m_LoadedIterator != other.m_LoadedIterator || m_MemoryIterator != other.m_MemoryIterator;
            }

        private:
            const AssetStorageType& m_LoadedAssets;
            const AssetStorageType& m_MemoryAssets;
            AssetStorageType::const_iterator m_LoadedIterator;
            AssetStorageType::const_iterator m_MemoryIterator;

            friend class AssetStorageIterator;
        };

        Iterator begin() const 
        {
            return Iterator( m_LoadedAssets, m_MemoryAssets, m_LoadedAssets.begin(), m_MemoryAssets.begin() );
        }

        Iterator end() const 
        {
            return Iterator( m_LoadedAssets, m_MemoryAssets, m_LoadedAssets.end(), m_MemoryAssets.end() );
        }

    private:
        const AssetStorageType& m_LoadedAssets;
        const AssetStorageType& m_MemoryAssets;

        template <typename T>
        friend class FilteredAssetStorageIterator;
    };

    template <typename T>
    class FilteredAssetStorageIterator {
    public:
        static_assert( std::is_base_of_v<Asset, T>, "T must inherit from Asset" );

        FilteredAssetStorageIterator( const AssetStorageType& a_LoadedAssets, const AssetStorageType& a_MemoryAssets )
            : m_LoadedAssets( a_LoadedAssets ), m_MemoryAssets( a_MemoryAssets ) {}

        FilteredAssetStorageIterator( const AssetStorageIterator& a_AssetStorageIterator )
            : m_LoadedAssets( a_AssetStorageIterator.m_LoadedAssets )
            , m_MemoryAssets( a_AssetStorageIterator.m_MemoryAssets ) {}

        // Iterator for filtered assets based on type T.
        class Iterator 
        {
        public:
            Iterator( 
                const AssetStorageType& a_LoadedAssets, const AssetStorageType& a_MemoryAssets,
                AssetStorageType::const_iterator a_LoadedIt, AssetStorageType::const_iterator a_MemoryIt )
                : m_LoadedAssets( a_LoadedAssets ), m_MemoryAssets( a_MemoryAssets )
                , m_LoadedIterator( a_LoadedIt ), m_MemoryIterator( a_MemoryIt )
            {
                SkipInvalid();
            }

            SharedPtr<T> operator*() const 
            {
                if ( m_LoadedIterator != m_LoadedAssets.end() )
                    return SharedPtrCast<T>( m_LoadedIterator->second );
                if ( m_MemoryIterator != m_MemoryAssets.end() )
                    return SharedPtrCast<T>( m_MemoryIterator->second );

                return nullptr;
            }

            Iterator& operator++() 
            {
                if ( m_LoadedIterator != m_LoadedAssets.end() ) 
                    ++m_LoadedIterator;
                else if ( m_MemoryIterator != m_MemoryAssets.end() ) 
                    ++m_MemoryIterator;

                SkipInvalid();
                return *this;
            }

            bool operator!=( const Iterator& other ) const 
            {
                return m_LoadedIterator != other.m_LoadedIterator 
                    || m_MemoryIterator != other.m_MemoryIterator;
            }

        private:
            const AssetStorageType& m_LoadedAssets;
            const AssetStorageType& m_MemoryAssets;
            AssetStorageType::const_iterator m_LoadedIterator;
            AssetStorageType::const_iterator m_MemoryIterator;

            void SkipInvalid() 
            {
                while ( m_LoadedIterator != m_LoadedAssets.end() &&
                    m_LoadedIterator->second->AssetType() != T::StaticType() ) 
                {
                    ++m_LoadedIterator;
                }
                while ( m_MemoryIterator != m_MemoryAssets.end() &&
                    m_MemoryIterator->second->AssetType() != T::StaticType() ) 
                {
                    ++m_MemoryIterator;
                }
            }

            friend class FilteredAssetStorageIterator;
        };

        Iterator begin() const 
        {
            return Iterator( m_LoadedAssets, m_MemoryAssets, m_LoadedAssets.begin(), m_MemoryAssets.begin() );
        }

        Iterator end() const 
        {
            return Iterator( m_LoadedAssets, m_MemoryAssets, m_LoadedAssets.end(), m_MemoryAssets.end() );
        }

    private:
        const AssetStorageType& m_LoadedAssets;
        const AssetStorageType& m_MemoryAssets;
    };

}