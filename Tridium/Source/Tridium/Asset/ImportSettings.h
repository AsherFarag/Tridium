#pragma once
#include <Tridium/Utils/Reflection/Reflection.h>

namespace Tridium {

	struct AssetImportSettings
	{
		REFLECT(AssetImportSettings);
	};

	struct ModelImportSettings : public AssetImportSettings
	{
		REFLECT( ModelImportSettings );
		ModelImportSettings();

		unsigned int PostProcessFlags;
		float Scale = 1.f;
		bool DiscardLocalData = false; /* If true, once the mesh has been loaded onto the GPU, the local Vertex Data will be deleted. */
	};

	BEGIN_REFLECT( AssetImportSettings );
	END_REFLECT( AssetImportSettings );

	BEGIN_REFLECT( ModelImportSettings );
		BASE( AssetImportSettings );
		PROPERTY( PostProcessFlags );
		PROPERTY( Scale );
		PROPERTY( DiscardLocalData );
	END_REFLECT( ModelImportSettings );

}