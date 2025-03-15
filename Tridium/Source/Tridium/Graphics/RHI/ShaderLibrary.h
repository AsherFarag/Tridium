#pragma once
#include "RHIShaderLibrary.h"

namespace Tridium {

	//=======================================================
	// Shader Library
	//  Static interface for the RHI shader library singleton.
	//  The shader library is used to manage and load shader modules.
	//  Shaders can be named and be retrieved by their name hash.
	class ShaderLibrary
	{
	public:
		// Find a shader module by its name hash.
		static RHIShaderModuleRef FindShader( hash_t a_NameHash )
		{
			return RHIShaderLibrary::Get()->FindShader( a_NameHash );
		}

		// Check if a shader module with the given name exists in the library.
		static bool HasShader( hash_t a_NameHash )
		{
			return RHIShaderLibrary::Get()->HasShader( a_NameHash );
		}

		// Load a shader module from a file and add it to the library with the given name.
		static RHIShaderModuleRef LoadShaderFromFile( const FilePath& a_Path, StringView a_Name = nullptr )
		{
			hash_t nameHash = a_Name.empty()
				? Hashing::HashString( a_Path.GetFilenameWithoutExtension().c_str() )
				: Hashing::HashString( a_Name.data() );

			if ( RHIShaderModuleRef ref = FindShader( nameHash ) )
			{
				LOG( LogCategory::RHI, Warn, "Shader '{0}' already loaded: {1}", ref->GetDescriptor()->Name.data(), a_Path.ToString() );
				return ref;
			}

			return RHIShaderLibrary::Get()->LoadShaderFromFile( a_Path, a_Name );
		}

		// Load a shader module from a string and add it to the library with the given name.
		static RHIShaderModuleRef LoadShader( StringView a_Source, StringView a_Name, ERHIShaderType a_Type )
		{
			hash_t nameHash = Hashing::HashString( a_Name.data() );
			if ( RHIShaderModuleRef ref = FindShader( nameHash ) )
			{
				LOG( LogCategory::RHI, Warn, "Shader '{0}' already loaded", a_Name.data() );
				return ref;
			}

			return RHIShaderLibrary::Get()->LoadShader( a_Source, a_Name, a_Type );
		}

		// Add a shader module to the library.
		static bool AddShader( CachedShader&& a_Shader )
		{
			return RHIShaderLibrary::Get()->AddShader( std::move( a_Shader ) );
		}

		// Add a shader module to the library.
		static bool AddShader( RHIShaderModuleRef a_Shader, StringView a_Name, String&& a_Source = {} )
		{
			return AddShader( CachedShader{ a_Name.data(), a_Shader, std::move( a_Source ) } );
		}
	};
	//=======================================================

} // namespace Tridium