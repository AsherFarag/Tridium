#include "tripch.h"
#include "AssetFactory.h"
#include <Tridium/Asset/AssetImporter.h>

namespace Tridium {

	decltype( AssetFactory::s_AssetTypes ) AssetFactory::s_AssetTypes;
	decltype( AssetFactory::s_AssetLoaders ) AssetFactory::s_AssetLoaders;

#if WITH_EDITOR

	decltype( AssetFactory::s_AssetImporters ) AssetFactory::s_AssetImporters;
	decltype( AssetFactory::s_ExtensionToImporterMap ) AssetFactory::s_ExtensionToImporterMap;

	bool AssetFactory::RegisterImporter( HashedString a_TypeHash, UniquePtr<IAssetImporter> a_Importer )
	{
		if ( s_AssetImporters.contains( a_TypeHash.Hash() ) )
		{
			ASSERT( false, "Asset Importer of type '{}' is already registered!", a_TypeHash.String() );
			return false;
		}

		for ( const StringView& ext : a_Importer->SupportedExtensions() )
		{
			if ( s_ExtensionToImporterMap.contains( ext ) )
			{
				ASSERT( false, "File extension '{}' is already registered to another importer!", ext );
				continue;
			}

			s_ExtensionToImporterMap[ ext ] = a_Importer.get();
		}

		s_AssetImporters[ a_TypeHash.Hash() ] = std::move( a_Importer );

		return true;
	}

#endif

} // namespace Tridium