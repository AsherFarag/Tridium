#pragma once
#include <Tridium/Asset/AssetDefinitions.h>
#include <Tridium/Core/Hash.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Utils/Concepts.h>
#include <Tridium/Containers/String.h>

namespace Tridium {

	#define ASSET_EXTENSION_NAME "tasset"
	constexpr StringView AssetExtensionName = ASSET_EXTENSION_NAME;

	//=================================================================================================
	// Define an Asset Type class.
	#define DEFINE_ASSET_TYPE( _Class ) \
		class _Class : public ::Tridium::AssetInterface<_Class>

	//=================================================================================================
	// Register an Asset Type with the Asset Factory.
	#define REGISTER_ASSET_TYPE( _Class, _AssetTypeInfo ) \
		DECLARE_INITIALIZER( AssetType_##_Class ) \
		DEFINE_INITIALIZER( AssetType_##_Class ) \
		{ \
			::Tridium::AssetFactory::RegisterAssetType<_Class>( _AssetTypeInfo ); \
		}

	template<typename T>
	using AssetRef = SharedPtr<T>;

	template<typename T>
	using AssetWeakRef = WeakPtr<T>;

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
		NON_COPYABLE_OR_MOVABLE( IAsset );
		virtual ~IAsset() = default;

		auto ID() const { return m_AssetID; }
		auto Flags() const { return m_AssetFlags; }

		virtual AssetTypeID Type() const = 0;
		virtual bool Valid() const { return true; }
		virtual void OnDependencyUpdated( AssetID a_DependencyID ) {}

		virtual bool operator==( const IAsset& a_Other ) const { return m_AssetID == a_Other.m_AssetID; }
		virtual bool operator!=( const IAsset& a_Other ) const { return !(*this == a_Other); }

	protected:

		//=============================================================================================
		IAsset() = default;

	protected:

		//=============================================================================================
		friend class AssetDatabase;
		friend class AssetFactory;

		//=============================================================================================
		AssetID m_AssetID{};
		EnumFlags<EAssetFlags> m_AssetFlags{};

	};

	//=================================================================================================
	// Asset Interface Template
	//=================================================================================================
	template<typename T>
	class AssetInterface : public IAsset
	{
	public:

		//=============================================================================================
		static AssetRef<T> Create( const AssetID a_AssetID, const EAssetFlags a_AssetFlags = EAssetFlags::None )
		{
			auto asset = MakeShared<T>();
			asset->m_AssetID = a_AssetID;
			asset->m_AssetFlags = a_AssetFlags;
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
		AssetID m_ID;

	public:

		//=============================================================================================
		using AssetType = T;

		//=============================================================================================
		AssetHandle() : m_ID( InvalidAssetID ), m_Ref( nullptr ) {}
		AssetHandle( AssetID a_AssetID ) : m_ID( a_AssetID ), m_Ref( nullptr ) {}
		AssetHandle( AssetRef<T> a_AssetRef ) : m_ID( InvalidAssetID ), m_Ref( std::move( a_AssetRef ) ) {}
		AssetHandle( std::nullptr_t ) : m_ID( InvalidAssetID ), m_Ref( nullptr ) {}

		//=============================================================================================
		bool Valid() const { return m_Ref ? true : m_ID != InvalidAssetID; }

		//=============================================================================================
		AssetID ID() const { return m_Ref ? m_Ref->ID() : m_ID; }

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