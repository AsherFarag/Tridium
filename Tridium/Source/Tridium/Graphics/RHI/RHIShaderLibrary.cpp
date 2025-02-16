#include "tripch.h"
#include "RHI.h"
#include "ShaderLibrary.h"
#include "ShaderPreprocessor.h"
#include <Tridium/IO/FileIO.h>

namespace Tridium {

    RHIShaderModuleRef RHIShaderLibrary::LoadShader( const FilePath& a_Path, StringView a_Name )
    {
		CachedShader cachedShader;
		cachedShader.Name = a_Name.empty() ? a_Path.GetFilenameWithoutExtension() : a_Name.data();

		// Read the source code from the file
		cachedShader.Source = IO::ReadFile( a_Path );
		if ( cachedShader.Source.empty() )
		{
			LOG( LogCategory::Rendering, Error, "Failed to load shader '{0}'", a_Path.ToString() );
			return nullptr;
		}

		// Preprocess the shader source to get the shader type
		PreprocessedShader preprocessed = ShaderPreprocessor::Process( cachedShader.Source );
		if ( !preprocessed.Error.empty() )
		{
			LOG( LogCategory::Rendering, Error, "Failed to preprocess shader '{0}': {1}", a_Path.ToString(), preprocessed.Error );
			return nullptr;
		}

		// Compile the shader
		ShaderCompilerInput input;
		input.Source = preprocessed.Source;
		input.ShaderType = preprocessed.Meta.ShaderType;
		ShaderCompilerOutput output = RHIShaderCompiler::Compile( input );
		if ( !output.IsValid() )
		{
			LOG( LogCategory::Rendering, Error, "Failed to compile shader '{0}': {1}", a_Path.ToString(), output.Error );
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
			LOG( LogCategory::Rendering, Error, "Failed to create shader module '{0}'", a_Path.ToString() );
			return nullptr;
		}
		cachedShader.Shader = shader;

		// Add the shader to the library
		m_CachedShaders[Hashing::Hash( cachedShader.Name.c_str() )] = std::move( cachedShader );

		return cachedShader.Shader;
    }

	RHIShaderModuleRef RHIShaderLibrary::LoadShader( StringView a_Source, StringView a_Name )
	{
		CachedShader cachedShader;
		if ( a_Name.empty() )
		{
			// If no name is provided, use an unnamed shader
			cachedShader.Name = "Unnamed_" + std::to_string( m_NextShaderID++ );
		}
		else
		{
			cachedShader.Name = a_Name;
		}

		cachedShader.Source = a_Source;

		// Preprocess the shader source to get the shader type
		PreprocessedShader preprocessed = ShaderPreprocessor::Process( cachedShader.Source );
		if ( !preprocessed.Error.empty() )
		{
			LOG( LogCategory::Rendering, Error, "Failed to preprocess shader '{0}': {1}", cachedShader.Name, preprocessed.Error );
			return nullptr;
		}

		// Construct the shader compiler input
		ShaderCompilerInput input;
		input.Source = preprocessed.Source;
		input.ShaderType = preprocessed.Meta.ShaderType;
		// Get the shader format from the RHI
		input.Format = RHIQuery::GetShaderFormat();

		// Compile the shader
		ShaderCompilerOutput output = RHIShaderCompiler::Compile( input );
		if ( !output.IsValid() )
		{
			LOG( LogCategory::Rendering, Error, "Failed to compile shader '{0}': {1}", cachedShader.Name, output.Error );
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

		return cachedShader.Shader;
	}

} // namespace Tridium