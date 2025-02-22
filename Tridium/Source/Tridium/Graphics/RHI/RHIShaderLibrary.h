#pragma once
#include "RHIShader.h"
#include "RHIShaderCompiler.h"
#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	//=======================================================
	// Cached Shader
	//  Holds the source and binary data for a shader module as the descriptors only hold references.
	struct CachedShader
	{
		String Name;
		RHIShaderModuleRef Shader;
		String Source;
		Array<Byte> Binary;
	};
	//=======================================================



	//=======================================================
	// RHI Shader Library
	//  Caches shader modules and provides a way to load and find them.
	class RHIShaderLibrary : public ISingleton<RHIShaderLibrary, /* _ExplicitSetup */ false>
	{
	public:
		// Load a shader module from a file and add it to the library with the given name.
		// If a name is not provided, the file name will be used.
		RHIShaderModuleRef LoadShader( const FilePath& a_Path, StringView a_Name = nullptr );

		// Load a shader module from a string and add it to the library with the given name.
		RHIShaderModuleRef LoadShader( StringView a_Source, StringView a_Name );

		// Find a shader module by its name hash.
		RHIShaderModuleRef FindShader( hash_t a_NameHash )
		{
			auto it = m_CachedShaders.find( a_NameHash );
			return it != m_CachedShaders.end() ? it->second.Shader : nullptr;
		}

		// Check if a shader module with the given name exists in the library.
		bool HasShader( hash_t a_NameHash ) const
		{
			return m_CachedShaders.find( a_NameHash ) != m_CachedShaders.end();
		}

		// Add a shader module to the library.
		bool AddShader( CachedShader&& a_Shader )
		{
			hash_t hash = Hashing::Hash( a_Shader.Name.c_str(), a_Shader.Name.size() );
			if ( HasShader( hash ) )
			{
				LOG( LogCategory::RHI, Warn, "Shader '{0}' already exists in the library", a_Shader.Name );
				return false;
			}

			if ( a_Shader.Name.empty() )
			{
				LOG( LogCategory::RHI, Warn, "Cannot add shader with empty name" );
				return false;
			}

			m_CachedShaders[hash] = std::move( a_Shader );
			return true;
		}

		// Add a shader module to the library.
		bool AddShader( RHIShaderModuleRef a_Shader, StringView a_Name, String&& a_Source = {}, Array<Byte>&& a_Binary = {} )
		{
			CachedShader cachedShader;
			cachedShader.Name = a_Name.empty() ? GenerateUniqueName() : a_Name.data();
			cachedShader.Shader = a_Shader;
			cachedShader.Source = a_Source;
			cachedShader.Binary = a_Binary;
			return AddShader( std::move( cachedShader ) );
		}

	private:
		UnorderedMap<hash_t, CachedShader> m_CachedShaders;
		// Used if a name is not provided when loading a shader.
		// Unnamed shaders are named "Unnamed_X" where X is the next ID.
		uint32_t m_NextShaderID = 0;

	private:
		String GenerateUniqueName()
		{
			String name = "Unnamed_" + std::to_string( m_NextShaderID++ );
			if ( HasShader( Hashing::Hash( name.c_str(), name.size() ) ) )
			{
				return GenerateUniqueName();
			}

			return name;
		}

	};

}