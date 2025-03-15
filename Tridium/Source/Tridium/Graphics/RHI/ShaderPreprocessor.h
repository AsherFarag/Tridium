#pragma once
#include "RHIDefinitions.h"
#include <Tridium/Containers/String.h>

namespace Tridium {

	struct PreprocessedShader
	{
		String Source{};
		String Error{};

		struct MetaData
		{
		} Meta;

		bool IsValid() const { return Error.empty(); }
	};

	//=======================================================
	// Shader Preprocessor
	//  Utility class for preprocessing shader source code.
	//  Used for handling Tridium-specific preprocessor directives.
	//  Such as retrieving the shader stage type from the source code.
	//  This class is not responsible for compiling the shader nor handling includes, etc.
	struct ShaderPreprocessor
	{
		PreprocessedShader Process( StringView a_Source );
	};
	//=======================================================

} // namespace Tridium