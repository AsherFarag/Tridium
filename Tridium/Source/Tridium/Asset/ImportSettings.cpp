#include "tripch.h"
#include "ImportSettings.h"
#include "assimp/postprocess.h"
#include <Tridium/Utils/Reflection/Reflection.h>

#define DEFAULT_POST_PROCESS_FLAGS aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GlobalScale

namespace Tridium {

	ModelImportSettings::ModelImportSettings()
		: PostProcessFlags( DEFAULT_POST_PROCESS_FLAGS )
	{
	}

	BEGIN_REFLECT( AssetImportSettings );
	END_REFLECT( AssetImportSettings );

	BEGIN_REFLECT( ModelImportSettings );
	    BASE( AssetImportSettings );
	    PROPERTY( PostProcessFlags );
	    PROPERTY( Scale );
	    PROPERTY( DiscardLocalData );
	END_REFLECT( ModelImportSettings );
}