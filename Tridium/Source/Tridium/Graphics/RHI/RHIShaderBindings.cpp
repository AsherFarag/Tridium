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
				bindingDesc = RHIShaderBinding::InlinedConstants( Cast<uint16_t>( binding.Size ) );
			}
			else
			{
				switch ( binding.Type )
				{
				case ShaderReflectionBinding::EType::ConstantBuffer: bindingDesc = RHIShaderBinding::ConstantBuffer( binding.Slot ); break;
				case ShaderReflectionBinding::EType::StructuredBuffer: bindingDesc = RHIShaderBinding::StructuredBuffer( binding.Slot ); break;
				case ShaderReflectionBinding::EType::StorageBuffer: bindingDesc = RHIShaderBinding::StorageBuffer( binding.Slot ); break;
				case ShaderReflectionBinding::EType::Texture: bindingDesc = RHIShaderBinding::Texture( binding.Slot, binding.TextureDimension ); break;
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