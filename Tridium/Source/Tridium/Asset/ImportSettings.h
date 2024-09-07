#pragma once

namespace Tridium {

	struct ImportSettings {};

	struct ModelImportSettings : public ImportSettings
	{
		ModelImportSettings();

		unsigned int PostProcessFlags;
		float Scale = 1.f;
		bool DiscardLocalData = false; /* If true, once the mesh has been loaded onto the GPU, the local Vertex Data will be deleted. */
	};

}