#pragma once
#include "RHIShader.h"
#include "RHIShaderCompiler.h"
#include <Tridium/Utils/Singleton.h>

namespace Tridium {

	class RHIShaderLibrary : public ISingleton<RHIShaderLibrary, /* _ExplicitSetup */ false>
	{
	public:
		// Holds the source and binary data for a shader module as the descriptors only hold references.
		struct CachedShader
		{
			String Name;
			RHIShaderModuleRef Shader;
			String Source;
			Array<Byte> Binary;
		};

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

	private:
		UnorderedMap<hash_t, CachedShader> m_CachedShaders;
		// Used if a name is not provided when loading a shader.
		// This is incremented for each shader loaded without a name.
		// Unnamed shaders are named "Unnamed_X" where X is the next ID.
		uint32_t m_NextShaderID = 0;
	};

}