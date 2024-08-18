#pragma once
#include "AssetMetaData.h"

namespace Tridium {

	struct ModelImportSettings
	{
		//PostProcessFlags = aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GlobalScale;
		unsigned int PostProcessFlags;
		float Scale = 1.f;
		bool DiscardLocalData = false; /* If true, once the mesh has been loaded onto the GPU, the local Vertex Data will be deleted. */
	};

	struct ModelMetaData : public AssetMetaData
	{
		ModelImportSettings ImportSettings;
	};

}