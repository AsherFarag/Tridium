#pragma once
#include <Tridium/Asset/AssetDefinitions.h>
#include <Tridium/Containers/UnorderedMap.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Core/Config.h>
#include <Tridium/Core/Types.h>
#include <Tridium/Core/Hash.h>

namespace Tridium {

	class IAsset;
	class IAssetLoader;
	class IAssetImporter;

	//=================================================================================================
	// Asset Factory: Global factory storing all asset importers and loaders.
	//=================================================================================================
	class AssetFactory
	{
	public:

		//=============================================================================================
		NON_COPYABLE_OR_MOVABLE( AssetFactory );

		//=============================================================================================
		// Attempts to retrieve the AssetTypeInfo for a given asset type ID.
		static const AssetTypeInfo& GetAssetTypeInfo( const AssetTypeID a_TypeID )
		{
			static const AssetTypeInfo s_InvalidTypeInfo{ .ID = InvalidAssetTypeID, .Name = "<UNKNOWN>" };
			auto it = Get().s_AssetTypes.find( a_TypeID );
			return it != Get().s_AssetTypes.end() ? it->second : s_InvalidTypeInfo;
		}

		//=============================================================================================
		// Gets the AssetTypeInfo for the asset type T.
		template<Concepts::Derived<IAsset> T>
		static const AssetTypeInfo* GetAssetTypeInfo()
		{
			static const AssetTypeInfo* s_CachedTypeInfo = GetAssetTypeInfo( Hashing::TypeHash<T>().Hash() );
			ASSERT( s_CachedTypeInfo, "Asset Type Info for type '{}' is not registered!", GetTypeName<T>() );
			return s_CachedTypeInfo;
		}

		//=============================================================================================
		// Registers an asset type T with the given AssetTypeInfo.
		// If 'a_Override' is true, it will overwrite any existing registration.
		template<Concepts::Derived<IAsset> T>
		static bool RegisterAssetType( AssetTypeInfo a_TypeInfo, bool a_Override = false )
		{
			constexpr AssetTypeID typeID = Hashing::TypeHash<T>().Hash();

			ASSERT( a_TypeInfo.ID == InvalidAssetTypeID || a_TypeInfo.ID == typeID,
					"AssetTypeInfo ID does not match the type hash for type '{}'.", GetTypeName<T>() );

			if ( !a_Override && Get().s_AssetTypes.contains( typeID ) )
			{
				return false;
			}

			a_TypeInfo.ID = typeID; // Ensure the ID matches the type hash
			Get().s_AssetTypes[ typeID ] = std::move( a_TypeInfo );

			return true;
		}


		//=============================================================================================
		// Attempts to retrieve a registered asset loader by its ID.
		static IAssetLoader* GetLoader( const AssetTypeID a_LoaderID ) 
		{ 
			auto it = Get().s_AssetLoaders.find( a_LoaderID );
			return it != Get().s_AssetLoaders.end() ? it->second.get() : nullptr;
		}

		//=============================================================================================
		// Gets the registered asset loader of type T.
		template<Concepts::Derived<IAssetLoader> T>
		static T* GetLoader()
		{
			static T* s_CachedLoader = DynamicCast<T*>( GetLoader( Hashing::TypeHash<T>().Hash() ) );
			ASSERT( s_CachedLoader, "Asset Loader of type '{}' is not registered!", GetTypeName<T>() );
			return s_CachedLoader;
		}

		//=============================================================================================
		// Registers an asset loader of type T. Returns true if registration was successful.
		template<Concepts::Derived<IAssetLoader> T>
		static bool RegisterLoader( const AssetTypeID a_LoaderID )
		{
			if ( Get().s_AssetLoaders.contains( a_LoaderID ) )
			{
				ASSERT( false, "Asset Loader of type '{}' is already registered!", GetTypeName<T>() );
				return false;
			}

			Get().s_AssetLoaders[a_LoaderID] = MakeUnique<T>();

			return true;
		}

	#if WITH_EDITOR

		//=============================================================================================
		// Attempts to retrieve a registered asset importer by a file extension it supports.
		static IAssetImporter* GetImporter( const StringView a_FileExtension )
		{
			const size_t strippedPrefix = a_FileExtension.find_first_not_of( '.' );
			const StringView removedDot = strippedPrefix != StringView::npos ? a_FileExtension.substr( strippedPrefix ) : a_FileExtension;
			auto it = Get().s_ExtensionToImporterMap.find( removedDot );
			return it != Get().s_ExtensionToImporterMap.end() ? it->second : nullptr;
		}

		//=============================================================================================
		// Gets the registered asset importer of type T.
		template<Concepts::Derived<IAssetImporter> T>
		static T* GetImporter()
		{
			static T* s_CachedImporter = DynamicCast<T*>( GetImporter( T::StaticImporterID() ) );
			ASSERT( s_CachedImporter, "Asset Importer of type '{}' is not registered!", GetTypeName<T>() );
			return s_CachedImporter;
		}

		//=============================================================================================
		// Registers an asset importer of type T. Returns true if registration was successful.
		template<Concepts::Derived<IAssetImporter> T>
		static bool RegisterImporter()
		{
			return RegisterImporter( Hashing::TypeHash<T>(), MakeUnique<T>() );
		}

	#endif

	private:

		//=============================================================================================
		AssetFactory() = default;
		~AssetFactory() = default;
		static AssetFactory& Get();

		//=============================================================================================
		UnorderedMap<AssetTypeID, AssetTypeInfo> s_AssetTypes;
		UnorderedMap<AssetTypeID, SharedPtr<IAssetLoader>> s_AssetLoaders;
		
	#if WITH_EDITOR

		//=============================================================================================
		UnorderedMap<AssetTypeID, SharedPtr<IAssetImporter>> s_AssetImporters;

		//=============================================================================================
		// Map of file extensions to their corresponding asset importers. E.g. "png" -> TextureImporter
		// Multiple extensions can map to the same importer.
		UnorderedMap<StringView, IAssetImporter*> s_ExtensionToImporterMap;

		static bool RegisterImporter( HashedString a_TypeHash, UniquePtr<IAssetImporter> a_Importer );

	#endif

	};

} // namespace Tridium