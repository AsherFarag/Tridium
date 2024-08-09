#pragma once
#include <Tridium/Core/Core.h>

namespace Tridium {

#define ASSET_CLASS_TYPE(type) static EAssetType GetStaticType() { return EAssetType::type; }\
							   virtual EAssetType GetAssetType() const { return GetStaticType(); }\
                               virtual const char* AssetTypeName() const { return #type; }\

	enum class EAssetType : unsigned char
	{
		None = 0, Mesh, Shader, Texture, Material,
	};

    typedef GUID AssetHandle;

    class Asset
    {
        friend class AssetManager;
    public:
        ASSET_CLASS_TYPE( None )

        const std::string& GetPath() const { return m_Path; }
        const AssetHandle& GetHandle() const { return m_Handle; }

        bool IsLoaded() const { return m_Loaded; }

    protected:
        void AddRef() { ++m_RefCount; }
        void Release();

    protected:
        AssetHandle m_Handle;
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
        AssetRef( const AssetHandle& a_AssetHandle );
        AssetRef( const AssetRef& other );
        AssetRef& operator=( const AssetRef& other );
        ~AssetRef();

        T* operator->() const { return m_Ptr; }
        T& operator*() const { return *m_Ptr; }
        T* Get() const { return m_Ptr; }
        void Reset();

        const AssetHandle& GetAssetHandle() const { return m_AssetHandle; }
        void SetAssetHandle( AssetHandle a_AssetHandle );

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
            m_Ptr->AddRef();
            m_AssetHandle = m_Ptr->GetAssetHandle();
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
        m_AssetHandle = other.m_AssetHandle;

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
        m_AssetHandle = AssetHandle::Null();
        if ( m_Ptr )
        {
            m_Ptr->Release();
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

#pragma endregion

#pragma endregion

}