#pragma once
#include <Tridium/Core/Core.h>

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
        ASSET_CLASS_TYPE( None )

        const std::string& GetPath() const { return m_Path; }
        const GUID GetGUID() const { return m_GUID; }

        bool IsLoaded() const { return m_Loaded; }

    protected:
        void AddRef() { ++m_RefCount; }
        void Release();

    protected:
        GUID m_GUID;
        std::string m_Path;
        size_t m_RefCount = 0;
        bool m_Loaded = false;
    };

#pragma region Asset Ref

    template <typename T>
    class AssetRef
    {
    public:
        AssetRef( T* a_Ptr = nullptr );
        AssetRef( const GUID& a_AssetGUID );
        AssetRef( const AssetRef& other );
        AssetRef& operator=( const AssetRef& other );
        ~AssetRef();

        T* operator->() const { return m_Ptr; }
        T& operator*() const { return *m_Ptr; }
        T* Get() const { return m_Ptr; }
        void Reset();

        GUID GetGUID() const { return m_AssetGUID; }
        void SetGUID( GUID a_GUID );

        explicit operator bool() const { return m_Ptr != nullptr; }

    private:
        GUID m_AssetGUID;
        T* m_Ptr = nullptr;
    };

    template<typename T>
    inline AssetRef<T>::AssetRef( T* a_Ptr )
        : m_Ptr( a_Ptr )
    {
        if ( m_Ptr )
        {
            m_Ptr->AddRef();
            m_AssetGUID = m_Ptr->GetGUID();
        }
    }

    template<typename T>
    inline AssetRef<T>::AssetRef( const GUID& a_AssetGUID )
        : m_AssetGUID( a_AssetGUID ) {}

    template<typename T>
    inline AssetRef<T>::AssetRef( const AssetRef& other )
        : m_Ptr( other.m_Ptr ), m_AssetGUID( other.m_AssetGUID )
    {
        if ( m_Ptr )
            m_Ptr->AddRef();
    }

    template<typename T>
    inline AssetRef<T>& AssetRef<T>::operator=( const AssetRef<T>& other )
    {
        if ( this == &other )
            return *this;

        if ( m_Ptr )
            m_Ptr->Release();

        m_Ptr = other.m_Ptr;
        m_AssetGUID = other.m_AssetGUID;

        if ( m_Ptr )
            m_Ptr->AddRef();

        return *this;
    }

    template<typename T>
    inline AssetRef<T>::~AssetRef()
    {
        if ( m_Ptr )
            m_Ptr->Release();
    }

    template<typename T>
    inline void AssetRef<T>::Reset()
    {
        m_AssetGUID = GUID::Null();
        if ( m_Ptr )
        {
            m_Ptr->Release();
            m_Ptr = nullptr;
        }
    }

    template<typename T>
    inline void AssetRef<T>::SetGUID( GUID a_GUID )
    {
        if ( m_AssetGUID == a_GUID )
            return;

        Reset();

        m_AssetGUID = a_GUID;
    }

#pragma endregion

}