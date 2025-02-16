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
		static RHIShaderModuleRef FindShader( hash_t a_NameHash )
		{
			return RHIShaderLibrary::Get()->FindShader( a_NameHash );
		}

		static bool HasShader( hash_t a_NameHash )
		{
			return RHIShaderLibrary::Get()->HasShader( a_NameHash );
		}

		static RHIShaderModuleRef LoadShader( const FilePath& a_Path, StringView a_Name = nullptr )
		{
			hash_t nameHash = a_Name.empty()
				? Hashing::Hash( a_Path.GetFilenameWithoutExtension().c_str() )
				: Hashing::Hash( a_Name.data() );

			if ( RHIShaderModuleRef ref = FindShader( nameHash ) )
			{
				LOG( LogCategory::RHI, Warn, "Shader '{0}' already loaded: {1}", ref->GetDescriptor()->Name.data(), a_Path.ToString() );
				return ref;
			}

			return RHIShaderLibrary::Get()->LoadShader( a_Path, a_Name );
		}

		static RHIShaderModuleRef LoadShader( StringView a_Source, StringView a_Name )
		{
			hash_t nameHash = Hashing::Hash( a_Name.data() );
			if ( RHIShaderModuleRef ref = FindShader( nameHash ) )
			{
				LOG( LogCategory::RHI, Warn, "Shader '{0}' already loaded", a_Name.data() );
				return ref;
			}

			return RHIShaderLibrary::Get()->LoadShader( a_Source, a_Name );
		}
	};
	//=======================================================

} // namespace Tridium