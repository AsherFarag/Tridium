#include "tripch.h"
#include "RHIShaderBindings.h"
#include <Tridium/Graphics/RHI/RHIShaderCompiler.h>

namespace Tridium {

	RHIBindingLayoutDesc RHI::BuildLayoutFromShader( String a_Name, const ShaderReflectionData& a_ShaderReflection )
	{
		RHIBindingLayoutDesc layoutDesc{};

		for ( const ShaderReflectionBinding& binding : a_ShaderReflection.Bindings )
		{
			RHIShaderBinding bindingDesc{};

			if ( binding.Space == INLINED_CONSTANTS_SPACE )
			{
				bindingDesc.AsInlinedConstants( binding.Slot, Cast<uint16_t>( binding.Size ) );
			}
			else
			{
				switch ( binding.Type )
				{
				case ShaderReflectionBinding::EType::ConstantBuffer: bindingDesc.AsConstantBuffer( binding.Slot ); break;
				case ShaderReflectionBinding::EType::StructuredBuffer: bindingDesc.AsStructuredBuffer( binding.Slot ); break;
				case ShaderReflectionBinding::EType::StorageBuffer: bindingDesc.AsStorageBuffer( binding.Slot ); break;
				case ShaderReflectionBinding::EType::Texture: bindingDesc.AsTexture( binding.Slot ); break;
				case ShaderReflectionBinding::EType::Sampler: continue; // We can safely ignore samplers as they are part of the texture binding in modern APIs.
				default:
					LOG( LogCategory::RHI, Warn, "Unknown binding type for resource '{0}'", binding.Name );
					continue; // Skip unknown types
				}
			}

			layoutDesc.AddBinding( HashedString( binding.Name ), bindingDesc );
		}

		return layoutDesc;
	}

}