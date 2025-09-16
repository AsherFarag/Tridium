#include "tripch.h"
#include "ShaderLibrary.h"
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/RHI/RHIShaderCompiler.h>

namespace Tridium {

	//=============================================================================================
	static UnorderedMap<name_t, ShaderFamily> s_ShaderFamilies;
	static UnorderedMap<hash64_t, ShaderFamilyVariant> s_ShaderVariants;

	bool ShaderLibrary::RegisterFamily( ShaderFamily&& a_Family )
	{
		name_t familyNameHash = Hashing::HashString( a_Family.Name );

		if ( s_ShaderFamilies.find( familyNameHash ) != s_ShaderFamilies.end() )
		{
			LOG( LogCategory::Rendering, Warn, "Shader family '{0}' is already registered", a_Family.Name );
			return false;
		}

		s_ShaderFamilies[familyNameHash] = std::move( a_Family );

		return true;
	}

	const ShaderFamily* ShaderLibrary::GetFamily( name_t a_FamilyName )
	{
		auto it = s_ShaderFamilies.find( a_FamilyName );

		if ( it != s_ShaderFamilies.end() )
		{
			return &it->second;
		}

		return nullptr;
	}

	const ShaderFamilyVariant* ShaderLibrary::GetOrCreateVariant( name_t a_FamilyName, const ShaderSwitchSet& a_Switches )
	{
		const ShaderFamily* family = GetFamily( a_FamilyName );

		if ( !family )
		{
			return nullptr;
		}

		// Compute the hash for the variant based on the family name and switches
		const hash64_t variantHash = Hashing::HashCombine( a_FamilyName, a_Switches.Hash() );

		// Check if the variant already exists
		auto it = s_ShaderVariants.find( variantHash );
		if ( it != s_ShaderVariants.end() )
		{
			return &it->second;
		}

		// Variant does not exist, create it
		ShaderFamilyVariant variant;
		variant.Switches = a_Switches;

		// Compile shaders for each stage
		for ( size_t i = 0; i < (size_t)ERHIShaderType::COUNT; ++i )
		{
			StringView source = family->ShaderSources[i];

			if ( source.empty() )
				continue; // No source for this stage

			// Construct the shader compiler input
			ShaderCompilerInput input;
			input.Source = source;
			input.ShaderType = (ERHIShaderType)i;
			input.Format = RHI::GetShaderFormat();

			auto output = RHIShaderCompiler::Compile( input );

			if ( output.IsError() )
			{
				LOG( LogCategory::Rendering, Error, "Failed to compile shader '{}' - Error: {}", family->Name, output.Error() );
				return nullptr;
			}

			RHIShaderModuleDesc desc;
			desc.Name = std::format( "{}_{}", family->Name, (size_t)input.ShaderType );
			desc.Type = input.ShaderType;
			desc.Bytecode = output.Value().ByteCode;
			desc.Source = input.Source;

			// Create the shader module
			variant.ShaderStages[i] = RHI::CreateShaderModule( desc );

			if ( !variant.ShaderStages[i] || !variant.ShaderStages[i]->Valid() )
			{
				LOG( LogCategory::Rendering, Error, "Failed to create shader module '{}'", family->Name );
				return nullptr;
			}
		}

		if ( !variant.Valid() )
		{
			LOG( LogCategory::Rendering, Error, "No valid shader stages compiled for variant of family '{}'", family->Name );
			return nullptr;
		}

		// Store the variant
		return &( s_ShaderVariants[variantHash] = std::move( variant ) );
	}

	bool ShaderLibrary::Init()
	{
		return true;
	}

	void ShaderLibrary::Shutdown()
	{
		s_ShaderFamilies.clear();
		s_ShaderVariants.clear();
	}

} // namespace Tridium