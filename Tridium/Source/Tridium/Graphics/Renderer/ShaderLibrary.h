#pragma once
#include <Tridium/Graphics/Renderer/ShaderFamily.h>
#include <Tridium/Utils/StaticInitializer.h>

namespace Tridium {

	//=================================================================================================
	// Helper macro to register built-in shader families at startup.
	#define REGISTER_SHADER_FAMILY( _Family ) \
		DECLARE_INITIALIZER(_Family); \
		DEFINE_INITIALIZER(_Family) { \
			const bool registered = ::Tridium::ShaderLibrary::RegisterFamily( ::Tridium::ShaderFamily{ _Family } ); \
			ASSERT( registered, "Failed to register shader family: {}", _Family.Name ); \
		}

	//=================================================================================================
	// Shader Library: Caches and manages shader families and their compiled variants.
	//=================================================================================================
	class ShaderLibrary
	{
	public:

		//=============================================================================================
		// Register a shader family with the library. 
		// Returns false if a family with the same name already exists.
		static bool RegisterFamily( ShaderFamily&& a_Family );

		//=============================================================================================
		// Retrieve a shader family by name. Returns nullptr if not found.
		static const ShaderFamily* GetFamily( name_t a_FamilyName );

		//=============================================================================================
		// Get or create a shader family variant based on the provided switches.
		// Returns nullptr if the family does not exist or if the variant could not be created.
		static const SharedPtr<ShaderFamilyVariant>& GetOrCreateVariant( name_t a_FamilyName, const ShaderSwitchSet& a_Switches = {} );

	private:

		//=============================================================================================
		friend class RendererModule;
		static bool Init();
		static void Shutdown();

	};

} // namespace Tridium