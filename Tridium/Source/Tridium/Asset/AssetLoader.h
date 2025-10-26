#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Asset/AssetFactory.h>
#include <Tridium/Core/Core.h>
#include <Tridium/Core/Version.h>
#include <Tridium/IO/IOStream.h>
#include <Tridium/Utils/StaticInitializer.h>

namespace Tridium {

	//=================================================================================================
	// Define an Asset Loader class.
	#define DEFINE_ASSET_LOADER( _Class ) \
		class _Class : public ::Tridium::AssetLoader<_Class>

	//=================================================================================================
	// Registered an Asset Importer to the Asset Importer Factory.
	#define REGISTER_ASSET_LOADER( _Class ) \
		DECLARE_INITIALIZER( AssetImporter_##_Class ) \
		DEFINE_INITIALIZER( AssetImporter_##_Class ) \
		{ \
			::Tridium::AssetFactory::RegisterLoader<_Class>( ::Tridium::Hashing::TypeHash<_Class>().Hash() ); \
		}

	//=================================================================================================
	// Asset Load Data: The data passed into an asset loader to load an asset.
	//=================================================================================================
	struct AssetLoadData
	{
		AssetHeader Header{};
		UUID ID{};
		Span<const byte_t> Data{};
	};

	//=================================================================================================
	// Asset Loader Interface: Base class for all asset loaders.
	// An asset loader is responsible for loading asset data from a byte stream into an asset instance.
	// To implement a custom loader, use the DEFINE_ASSET_LOADER macro to define your loader class,
	// then register it with the AssetFactory using REGISTER_ASSET_LOADER( MyCustomLoader ) in a cpp file.
	//=================================================================================================
	class IAssetLoader
	{
	public:

		//=============================================================================================
		virtual ~IAssetLoader() = default;

		//=============================================================================================
		virtual UniquePtr<IAsset> Create() const = 0;

		//=============================================================================================
		// Loads the asset data into 'o_Asset'. Returns an error string on failure.
		virtual Expected<void, String> Load( const AssetLoadData& a_LoadData, IAsset& o_Asset ) = 0;

		//=============================================================================================
		// Serializes the asset data into the stream. Thi
		virtual Expected<void, String> Save( const IAsset& a_Asset, IOutputStream& a_Stream ) = 0;

	};

	//=================================================================================================
	// Asset Loader Template
	//=================================================================================================
	template<Concepts::Derived<IAsset> T>
	class AssetLoader : public IAssetLoader
	{
	public:

		//=============================================================================================
		using AssetType = T;

		//=============================================================================================
		UniquePtr<IAsset> Create() const override
		{
			return MakeUnique<T>();
		}

	};

} // namespace Tridium