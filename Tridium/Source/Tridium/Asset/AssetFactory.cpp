#include "tripch.h"
#include "AssetFactory.h"
#include <Tridium/Asset/AssetImporter.h>

namespace Tridium {

	AssetFactory& AssetFactory::Get()
	{
		static AssetFactory s_Instance;
		return s_Instance;
	}

#if WITH_EDITOR

	bool AssetFactory::RegisterImporter( HashedString a_TypeHash, UniquePtr<IAssetImporter> a_Importer )
	{
		if ( Get().s_AssetImporters.contains( a_TypeHash.Hash() ) )
		{
			ASSERT( false, "Asset Importer of type '{}' is already registered!", a_TypeHash.String() );
			return false;
		}

		for ( const StringView& ext : a_Importer->SupportedExtensions() )
		{
			if ( Get().s_ExtensionToImporterMap.contains( ext ) )
			{
				ASSERT( false, "File extension '{}' is already registered to another importer!", ext );
				continue;
			}

			Get().s_ExtensionToImporterMap[ ext ] = a_Importer.get();
		}

		Get().s_AssetImporters[ a_TypeHash.Hash() ] = std::move( a_Importer );

		return true;
	}

#endif

} // namespace Tridium