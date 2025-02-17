#include "tripch.h"
#include "RHI.h"
#include "ShaderLibrary.h"
#include "ShaderPreprocessor.h"
#include <Tridium/IO/FileIO.h>

namespace Tridium {

    RHIShaderModuleRef RHIShaderLibrary::LoadShader( const FilePath& a_Path, StringView a_Name )
    {
		// Read the source code from the file
		String source = IO::ReadFile( a_Path );
		if ( source.empty() )
		{
			LOG( LogCategory::Rendering, Error, "Failed to load shader from file '{0}'", a_Path.ToString() );
			return nullptr;
		}

		if ( a_Name.empty() )
		{
			// If no name is provided, use the file name
			return LoadShader( StringView( source ), StringView( a_Path.GetFilenameWithoutExtension() ) );
		}

		// Load the shader with the provided name
		return LoadShader( StringView( source ), a_Name );
    }

	RHIShaderModuleRef RHIShaderLibrary::LoadShader( StringView a_Source, StringView a_Name )
	{
		CachedShader cachedShader;
		if ( a_Name.empty() )
		{
			// If no name is provided, use an unnamed shader
			cachedShader.Name = GenerateUniqueName();
		}
		else
		{
			cachedShader.Name = a_Name;
		}

		cachedShader.Source = a_Source;

		// Preprocess the shader source to get the shader type
		ShaderPreprocessor preprocessor;
		PreprocessedShader preprocShader = preprocessor.Process( cachedShader.Source );
		if ( !preprocShader.Error.empty() )
		{
			LOG( LogCategory::Rendering, Error, "Failed to preprocess shader '{0}' - Error: {1}", cachedShader.Name, preprocShader.Error );
			return nullptr;
		}

		// Construct the shader compiler input
		ShaderCompilerInput input;
		input.Source = preprocShader.Source;
		input.ShaderType = preprocShader.Meta.ShaderType;
		// Get the shader format from the RHI
		input.Format = RHIQuery::GetShaderFormat();

		// Compile the shader
		ShaderCompilerOutput output = RHIShaderCompiler::Compile( input );
		if ( !output.IsValid() )
		{
			LOG( LogCategory::Rendering, Error, "Failed to compile shader '{0}' - Error: {1}", cachedShader.Name, output.Error );
			return nullptr;
		}
		cachedShader.Binary = std::move( output.ByteCode );

		// Construct the shader module descriptor
		RHIShaderModuleDescriptor desc;
		desc.Name = cachedShader.Name;
		desc.Type = input.ShaderType;
		desc.Binary = cachedShader.Binary;
		desc.Source = cachedShader.Source;

		// Create the shader module
		RHIShaderModuleRef shader = RHI::CreateShaderModule( desc );
		if ( !shader )
		{
			LOG( LogCategory::Rendering, Error, "Failed to create shader module '{0}'", cachedShader.Name );
			return nullptr;
		}
		cachedShader.Shader = shader;

		// Add the shader to the library
		m_CachedShaders[Hashing::Hash( cachedShader.Name.c_str() )] = std::move( cachedShader );
		return shader;
	}

} // namespace Tridium