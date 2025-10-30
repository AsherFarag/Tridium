#pragma once
#include <Tridium/Asset/AssetDefinitions.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Common/TimeStamp.h>
#include <Tridium/Containers/String.h>
#include <Tridium/Containers/UnorderedSet.h>
#include <Tridium/Core/Hash.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Utils/Concepts.h>
#include <Tridium/Utils/StaticInitializer.h>

namespace Tridium {

	//=================================================================================================
	// Define an Asset Type class.
	#define DEFINE_ASSET_TYPE( _Class ) \
		class _Class : public ::Tridium::AssetInterface<_Class>

	//=================================================================================================
	// Register an Asset Type with the Asset Factory.
	#define REGISTER_ASSET_TYPE( _Class, _AssetTypeInfo ) \
		DECLARE_INITIALIZER( AssetType_##_Class ); \
		DEFINE_INITIALIZER( AssetType_##_Class ) \
		{ \
			::Tridium::AssetFactory::RegisterAssetType<_Class>( _AssetTypeInfo ); \
		}

	template<typename T>
	using AssetRef = SharedPtr<T>;

	template<typename T>
	using AssetWeakRef = WeakPtr<T>;

	//=================================================================================================
	// Asset Info: Serialized metadata for an asset.
	//=================================================================================================
	struct AssetInfo
	{
		UUID ID{};
		AssetTypeID Type = InvalidAssetTypeID;
		EAssetLoadPolicy LoadPolicy = EAssetLoadPolicy::Default;
		EnumFlags<EAssetFlags> m_AssetFlags{};
		String Name{};
		String Path{};
		UnorderedSet<UUID> Dependencies{};

	#if WITH_EDITOR
		struct
		{
			// The original source file path from which the asset was imported from.
			String SourceFilePath{};
			// The timestamp of when the asset was created.
			TimeStamp CreationTime{};
			// The timestamp of when the asset was last modified.
			TimeStamp LastModifiedTime{};
		} Editor{};
	#endif

		bool Valid() const { return ID.Valid() && Type != InvalidAssetTypeID; }
	};

	//=================================================================================================
	// Asset Interface: Base class for all assets.
	// An asset represents a game resource, such as a texture, model, sound, etc.
	// It contains the data and functionality to manage the resource.
	// Assets are managed by the AssetDatabase.
	//=================================================================================================
	class IAsset : public EnableSharedFromThis<IAsset>
	{
	public:

		//=============================================================================================
		NON_COPYABLE( IAsset );
		IAsset( IAsset&& ) = default;
		IAsset& operator=( IAsset&& ) = default;
		virtual ~IAsset() = default;

		//=============================================================================================
		const SharedPtr<AssetInfo>& Info() const { return m_Info; }
		UUID ID() const { return m_Info->ID; }

		//=============================================================================================
		virtual AssetTypeID Type() const = 0;
		virtual bool Valid() const { return m_Info != nullptr && m_Info->Valid(); }
		virtual void OnDependencyUpdated( UUID a_DependencyID ) {}

	protected:

		//=============================================================================================
		IAsset() = default;

		//=============================================================================================
		friend class AssetDatabase;
		friend class AssetFactory;

		//=============================================================================================
		SharedPtr<AssetInfo> m_Info{};

	};

	//=================================================================================================
	// Asset Interface Template
	//=================================================================================================
	template<typename T>
	class AssetInterface : public IAsset
	{
	public:

		//=============================================================================================
		static AssetRef<T> Create( SharedPtr<AssetInfo> a_Info )
		{
			// Sanity check the asset type
			a_Info->Type = T::StaticType();
			AssetRef<T> asset = MakeShared<T>();
			asset->m_Info = std::move( a_Info );
			return asset;
		}

		//=============================================================================================
		static constexpr AssetTypeID StaticType() { return Hashing::TypeHash<T>(); }
		AssetTypeID Type() const override final { return StaticType(); }

	};

	//=================================================================================================
	// Asset Handle: A handle to an asset that can either store an AssetID or a direct reference to the asset.
	// This allows for lazy loading of assets, where the asset is only loaded when needed.
	// Load functions are defined in AssetDatabase.h.
	//=================================================================================================
	template<Concepts::Derived<IAsset> T>
	struct AssetHandle
	{
	private:

		//=============================================================================================
		AssetRef<T> m_Ref;
		UUID m_ID;

	public:

		//=============================================================================================
		using AssetType = T;

		//=============================================================================================
		AssetHandle() : m_ID(), m_Ref( nullptr ) {}
		AssetHandle( UUID a_AssetID ) : m_ID( a_AssetID ), m_Ref( nullptr ) {}
		AssetHandle( AssetRef<T> a_AssetRef ) : m_ID(), m_Ref( std::move( a_AssetRef ) ) {}
		AssetHandle( std::nullptr_t ) : m_ID(), m_Ref( nullptr ) {}

		//=============================================================================================
		bool Valid() const { return m_Ref ? true : m_ID.Valid(); }

		//=============================================================================================
		UUID ID() const { return m_Ref ? m_Ref->ID() : m_ID; }

		//=============================================================================================
		const AssetInfo* Info() const;

		//=============================================================================================
		const AssetRef<T>& Get() const { return m_Ref; }
		const AssetRef<T>& GetOrLoad();

		//=============================================================================================
		template<Concepts::Derived<IAsset> U>
		AssetHandle<U> Cast()
		{
			if ( m_Ref )
			{
				TODO( "Should be a dynamic cast with error checking." );
				return AssetHandle<U>( SharedPtrCast<U>( m_Ref ) );
			}
			else
			{
				return AssetHandle<U>( m_ID );
			}
		
		}

	};

} // namespace Tridium