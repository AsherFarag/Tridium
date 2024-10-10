#pragma once
#include <Tridium/Utils/Reflection/ReflectionFwd.h>

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

}