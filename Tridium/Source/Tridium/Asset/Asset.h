#pragma once
#include <string>
#include "AssetType.h"

namespace Tridium {

#define ASSET_CLASS_TYPE(type) static constexpr EAssetType StaticType() { return EAssetType::type; }\
							   virtual EAssetType AssetType() const { return StaticType(); }\
                               virtual constexpr const char* AssetTypeName() const { return #type; }\

#define ASSET_LOADER_TYPE(type) friend class type; \
                                using LoaderType = type;

#ifdef IS_EDITOR
    namespace Editor {
        class EditorAssetManager; // Forward declaration
    }
#endif // IS_EDITOR

    class Asset
    {
    public:
        ASSET_CLASS_TYPE( None )

        const std::string& GetPath() const { return m_Path; }
		void SetPath( const std::string& a_Path ) { m_Path = a_Path; }
        const AssetHandle& GetHandle() const { return m_Handle; }

        bool IsLoaded() const { return m_Loaded; }

        size_t GetRefCount() const { return m_RefCount; }

    protected:
        void Internal_AddRef() { ++m_RefCount; }
        void Internal_ReleaseRef()
        {
            if ( --m_RefCount == 0 )
            {
                // Clean up the asset if reference count drops to zero
                delete this;
            }
        }

    protected:
        AssetHandle m_Handle;
        std::string m_Path;
        size_t m_RefCount = 0;
        bool m_Loaded = false;

    private:
        template<typename T>
        friend class AssetRef;
        friend class AssetManager;

    #ifdef IS_EDITOR
        friend class Editor::EditorAssetManager;
    #endif // IS_EDITOR
    };

    template <typename T>
    class AssetRef
    {
    public:
        AssetRef( T* a_Ptr = nullptr );
        AssetRef( const AssetHandle& a_AssetHandle );
        AssetRef( const AssetRef& other );
        AssetRef& operator=( const AssetRef& other );
        ~AssetRef();

        T* operator->() const { return m_Ptr; }
        T& operator*() const { return *m_Ptr; }
        T* Get() const { return m_Ptr; }
        void Reset();

        template <typename AsT>
        AssetRef<AsT> As();

        const AssetHandle& GetAssetHandle() const { return m_AssetHandle; }
        void SetAssetHandle( AssetHandle a_AssetHandle );

        bool operator==( std::nullptr_t ) const { return m_Ptr == nullptr; }
        bool operator!=( std::nullptr_t ) const { return m_Ptr != nullptr; }
        explicit operator bool() const { return m_Ptr != nullptr; }

    private:
        AssetHandle m_AssetHandle;
        T* m_Ptr = nullptr;
    };

#pragma region Template Definitions

    template<typename T>
    inline AssetRef<T>::AssetRef( T* a_Ptr )
        : m_Ptr( a_Ptr )
    {
        if ( m_Ptr )
        {
            m_Ptr->Internal_AddRef();
            m_AssetHandle = m_Ptr->GetHandle();
        }
    }

    template<typename T>
    inline AssetRef<T>::AssetRef( const AssetHandle& a_AssetAssetHandle )
        : m_AssetHandle( a_AssetAssetHandle ) {}

    template<typename T>
    inline AssetRef<T>::AssetRef( const AssetRef& other )
        : m_Ptr( other.m_Ptr ), m_AssetHandle( other.m_AssetHandle )
    {
        if ( m_Ptr )
            m_Ptr->Internal_AddRef();
    }

    template<typename T>
    inline AssetRef<T>& AssetRef<T>::operator=( const AssetRef<T>& other )
    {
        if ( this == &other )
            return *this;

        if ( m_Ptr )
            m_Ptr->Internal_ReleaseRef();

        m_Ptr = other.m_Ptr;
        m_AssetHandle = other.m_AssetHandle;

        if ( m_Ptr )
            m_Ptr->Internal_AddRef();

        return *this;
    }

    template<typename T>
    inline AssetRef<T>::~AssetRef()
    {
        if ( m_Ptr )
            m_Ptr->Internal_ReleaseRef();
    }

    template<typename T>
    inline void AssetRef<T>::Reset()
    {
        m_AssetHandle = AssetHandle::Null();
        if ( m_Ptr )
        {
            m_Ptr->Internal_ReleaseRef();
            m_Ptr = nullptr;
        }
    }

    template<typename T>
    inline void AssetRef<T>::SetAssetHandle( AssetHandle a_AssetHandle )
    {
        if ( m_AssetHandle == a_AssetHandle )
            return;

        Reset();

        m_AssetHandle = a_AssetHandle;
    }

    template<typename T>
    template<typename AsT>
    inline AssetRef<AsT> AssetRef<T>::As()
    {
        if ( m_Ptr == nullptr )
            return nullptr;

        if constexpr ( Asset::StaticType() == AsT::StaticType() )
            return static_cast<Asset*>( m_Ptr );

        if ( AsT::StaticType() != m_Ptr->AssetType() )
            return nullptr;

        return static_cast<AsT*>(m_Ptr);
    }

#pragma endregion

}