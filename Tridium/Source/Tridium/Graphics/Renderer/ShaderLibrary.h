#pragma once
#include <Tridium/Graphics/RHI/RHIShader.h>
#include <Tridium/Graphics/Renderer/ShaderFamily.h>

namespace Tridium {

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
		static const ShaderFamilyVariant* GetOrCreateVariant( name_t a_FamilyName, const ShaderSwitchSet& a_Switches );

	private:

		//=============================================================================================
		friend class RendererModule;
		static bool Init();
		static void Shutdown();

	};

} // namespace Tridium