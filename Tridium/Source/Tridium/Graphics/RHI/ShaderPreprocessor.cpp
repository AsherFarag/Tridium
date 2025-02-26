#include "tripch.h"
#include "ShaderPreprocessor.h"

namespace Tridium {

    static constexpr StringView ShaderTypeToken = "#pragma type";
	static const UnorderedMap<hash_t, ERHIShaderType> ShaderTypes = 
    { 
		{ "vertex"_H,        ERHIShaderType::Vertex },
		{ "hull"_H,          ERHIShaderType::Hull },
		{ "domain"_H,        ERHIShaderType::Domain },
		{ "geometry"_H,      ERHIShaderType::Geometry },
        { "pixel"_H,         ERHIShaderType::Pixel },
		{ "compute"_H ,      ERHIShaderType::Compute }
    };

    PreprocessedShader ShaderPreprocessor::Process( StringView a_Source )
    {
		PreprocessedShader output;
		String source( a_Source );

		size_t typeTokenPos = source.find( ShaderTypeToken, 0 );
        if ( typeTokenPos == String::npos )
        {
            output.Error = "Shader type not specified";
            return output;
        }

		size_t eol = source.find_first_of( "\r\n", typeTokenPos );
        if ( eol == String::npos )
        {
            output.Error = "Invalid shader type directive";
            return output;
        }

        size_t begin = typeTokenPos + ShaderTypeToken.size();
		String type = Util::ToLowerCase( source.substr( begin, eol - begin ) );
		// Remove leading and trailing whitespace
		type = type.substr( type.find_first_not_of( " \t" ) );

		hash_t typeHash = Hashing::HashString( type.c_str() );

		auto it = ShaderTypes.find( typeHash );
        if ( it == ShaderTypes.end() )
        {
            output.Error = "Invalid shader type";
            return output;
        }

		output.Meta.ShaderType = it->second;
		output.Source = source.substr( eol + 1 );

		return output;
    }

} // namespace Tridium
