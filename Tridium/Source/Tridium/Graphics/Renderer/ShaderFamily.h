#pragma once
#include <Tridium/Core/Hash.h>
#include <Tridium/Containers/InitList.h>
#include <Tridium/Containers/UnorderedSet.h>
#include <Tridium/Graphics/RHI/RHIDefinitions.h>
#include <Tridium/Graphics/RHI/RHIShader.h>
#include <Tridium/Graphics/RHI/RHIPipelineState.h>

namespace Tridium {

	//=================================================================================================
	// Shader Switch Set: A set of shader switches that can be used to define different shader variants.
	// A shader switch is simply a preprocessor definition that can be toggled on or off.
	// E.g. ShaderSwitchSet.Set("USE_SHADOWS") will define the preprocessor definition USE_SHADOWS.
	//=================================================================================================
	struct ShaderSwitchSet
	{
		//=============================================================================================
		// The set of switches.
		UnorderedSet<String> Switches;

		//=============================================================================================
		ShaderSwitchSet() = default;
		ShaderSwitchSet( InitList<String> a_Switches ) : Switches( a_Switches ) {}

		//=============================================================================================
		// Returns true if a switch is enabled.
		bool IsSet( StringView a_Switch ) const
		{
			return Switches.contains( a_Switch );
		}

		//=============================================================================================
		// Enables a switch.
		void Set( String a_Switch )
		{
			Switches.emplace( std::move( a_Switch ) );
		}

		//=============================================================================================
		// Disables a switch (if it exists).
		void Unset( StringView a_Switch )
		{
			if ( auto it = Switches.find( a_Switch ); it != Switches.end() )
			{
				Switches.erase( it );
			}
		}

		//=============================================================================================
		// Clears all switches.
		void Clear()
		{
			Switches.clear();
		}

		//=============================================================================================
		// Computes the hash for the current set of switches.
		hash64_t Hash() const
		{
			// Order of switches does not matter, so we combine their hashes in a commutative way.
			std::hash<String> hasher;
			hash64_t result = 0;

			for ( auto& s : Switches )
			{
				result ^= hasher( s ) + 0x9e3779b97f4a7c15ULL + ( result << 6 ) + ( result >> 2 ); // boost hash combine
			}

			return result;
		}

	};

	//=================================================================================================
	// Shader Family Variant: A compiled variant of a shader family with specific switches.
	//=================================================================================================
	struct ShaderFamilyVariant
	{
		//=============================================================================================
		// The set of switches that were used to compile this variant.
		ShaderSwitchSet Switches;

		//=============================================================================================
		// The compiled shader modules for each shader type.
		RHIShaderModuleRef ShaderStages[(size_t)ERHIShaderType::COUNT];

		//=============================================================================================
		// The binding layout that this shader family variant uses.
		RHIBindingLayoutRef BindingLayout;

		//=============================================================================================
		// Returns true if at least one shader stage is valid.
		bool Valid() const
		{
			for ( const auto& shader : ShaderStages )
			{
				if ( shader && shader->Valid() )
				{
					return true;
				}
			}

			return false;
		}

		//=============================================================================================
		void Apply( RHIGraphicsPipelineStateDesc& a_PipelineDesc ) const
		{
			for ( size_t i = 0; i < (size_t)ERHIShaderType::COUNT; ++i )
			{
				if ( ShaderStages[i] && ShaderStages[i]->Valid() )
				{
					a_PipelineDesc.SetShader( (ERHIShaderType)i, ShaderStages[i] );
				}
			}

			a_PipelineDesc.AddBindingLayout( BindingLayout );
		}

	};

	//=================================================================================================
	// Shader Family: A collection of shader sources which shader variants can be compiled from.
	//=================================================================================================
	struct ShaderFamily
	{
		//=============================================================================================
		// Human-friendly name of the family.
		String Name;

		//=============================================================================================
		// List of the available switches for this shader family. If a variant is requested with
		// switches not in this list, they will be ignored.
		ShaderSwitchSet DefaultSwitches;

		//=============================================================================================
		// The shader source code for each shader type.
		StringView ShaderSources[(size_t)ERHIShaderType::COUNT];

		auto& SetName( String a_Name ) { Name = std::move( a_Name ); return *this; }
		auto& SetDefaultSwitches( const ShaderSwitchSet& a_Switches ) { DefaultSwitches = a_Switches; return *this; }
		auto& SetShaderSource( ERHIShaderType a_Type, StringView a_Source ) { ShaderSources[(size_t)a_Type] = a_Source; return *this; }
	};

} // namespace Tridium