#include "tripch.h"
#include "ShaderLibrary.h"
#include <Tridium/Graphics/RHI/ShaderPreprocessor.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/IO/FileIO.h>

namespace Tridium {

	ERHIShaderType GetShaderTypeFromFileName( StringView a_FileName )
	{
		if ( a_FileName.size() < 2 )
			return ERHIShaderType::Unknown;

		// We only need the first letter of the suffix as the second letter is always 'S' for shader
		char shaderSuffix[2] = { a_FileName[a_FileName.size() - 2], a_FileName[a_FileName.size() - 1] };

		switch ( std::tolower( shaderSuffix[0] ) )
		{
			case 'v': return ERHIShaderType::Vertex;
			case 'h': return ERHIShaderType::Hull;
			case 'd': return ERHIShaderType::Domain;
			case 'g': return ERHIShaderType::Geometry;
			case 'p': return ERHIShaderType::Pixel;
			case 'c': return ERHIShaderType::Compute;
		}

		ASSERT( false, "Unknown shader type suffix '{0}' while loading shader '{1}'", shaderSuffix, a_FileName );
		return ERHIShaderType::Unknown;
	}

	RHIShaderModuleRef ShaderLibrary::LoadShaderFromFile( const FilePath& a_Path, StringView a_Name, ERHIShaderType a_Type )
    {
		hash_t nameHash = a_Name.empty()
			? Hashing::HashString( a_Path.GetFilenameWithoutExtension().c_str() )
			: Hashing::HashString( a_Name.data() );

		if ( RHIShaderModuleRef ref = FindShader( nameHash ) )
		{
			LOG( LogCategory::RHI, Warn, "Shader '{0}' already loaded: {1}", ref->Desc().Name.data(), a_Path.ToString() );
			return ref;
		}

		// Read the source code from the file
		String source = IO::ReadFile( a_Path.ToString() );
		if ( source.empty() )
		{
			LOG( LogCategory::Rendering, Error, "Failed to load shader from file '{0}'", a_Path.ToString() );
			return nullptr;
		}

		if ( a_Type == ERHIShaderType::Unknown )
		{
			// Determine the shader type from the file name
			a_Type = GetShaderTypeFromFileName( a_Path.GetFilenameWithoutExtension() );
			if ( a_Type == ERHIShaderType::Unknown )
			{
				return nullptr;
			}
		}

		if ( a_Name.empty() )
		{
			// If no name is provided, use the file name
			return LoadShader( StringView( source ), a_Path.GetFilenameWithoutExtension(), a_Type );
		}

		// Load the shader with the provided name
		return LoadShader( StringView( source ), a_Name, a_Type );
    }

	RHIShaderModuleRef ShaderLibrary::LoadShader( StringView a_Source, StringView a_Name, ERHIShaderType a_Type )
	{
		hash_t nameHash = Hashing::HashString( a_Name.data() );
		if ( RHIShaderModuleRef ref = FindShader( nameHash ) )
		{
			LOG( LogCategory::RHI, Warn, "Shader '{0}' already loaded", a_Name.data() );
			return ref;
		}

		CachedShader cachedShader;
		cachedShader.Name = a_Name.empty() ? GenerateUniqueName() : String( a_Name );
		cachedShader.Source = a_Source;

		// Construct the shader compiler input
		ShaderCompilerInput input;
		input.Source = cachedShader.Source;
		input.ShaderType = a_Type;
		// Get the shader format from the RHI
		input.Format = RHI::GetShaderFormat();

		// Compile the shader
		auto output = RHIShaderCompiler::Compile( input );
		if ( output.IsError() )
		{
			LOG( LogCategory::Rendering, Error, "Failed to compile shader '{0}' - Error: {1}", cachedShader.Name, output.Error() );
			return nullptr;
		}

		// Construct the shader module descriptor
		RHIShaderModuleDesc desc;
		desc.Name = cachedShader.Name;
		desc.Type = input.ShaderType;
		desc.Bytecode = output.Value().ByteCode;
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
		m_CachedShaders[Hashing::HashString( cachedShader.Name )] = std::move( cachedShader );
		return shader;
	}

} // namespace Tridium