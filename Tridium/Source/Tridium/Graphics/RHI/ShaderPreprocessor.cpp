#include "tripch.h"
#include "ShaderPreprocessor.h"

namespace Tridium {

    PreprocessedShader ShaderPreprocessor::Process( StringView a_Source )
    {
		PreprocessedShader output;
		output.Source = a_Source;
		return output;
    }

} // namespace Tridium
