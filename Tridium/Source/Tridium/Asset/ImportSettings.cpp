#include "tripch.h"
#include "ImportSettings.h"
#include "assimp/postprocess.h"

#define DEFAULT_POST_PROCESS_FLAGS aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GlobalScale

namespace Tridium {

	ModelImportSettings::ModelImportSettings()
		: PostProcessFlags( DEFAULT_POST_PROCESS_FLAGS )
	{
	}

}