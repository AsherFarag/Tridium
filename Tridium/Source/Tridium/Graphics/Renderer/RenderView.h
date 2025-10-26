#pragma once
#include <Tridium/Core/Handle.h>
#include <Tridium/Graphics/Renderer/Lighting.h>
#include <Tridium/Graphics/RHI/RHIDefinitions.h>
#include <Tridium/Shaders/RenderView_ShaderInterop.h>

namespace Tridium {

	//=============================================================================================
	// Render View Type: The type of render view,
	// which determines how it is processed by the render pipeline.
	// Custom views can be defined for specialized rendering tasks, 
	// but are ignored by default pipeline processes.
	//=============================================================================================
	enum class ERenderViewType : uint8_t
	{
		None = 0,
		Camera,
		Shadow,
		Custom,
	};

	//=============================================================================================
	// Render View: Represents a specific viewpoint or camera through which draw items are rendered.
	//=============================================================================================
	struct RenderView
	{
		//=========================================================================================
		// View constants the shaders will use when rendering this view.
		RenderViewConstants Constants{};

		//=========================================================================================
		// A constant buffer containing the view constants. If null, it will be created at render time.
		RHIBufferRef ConstantsBuffer = nullptr;

		//=========================================================================================
		// Optional name for debugging and profiling purposes.
		String Name{};

		//=========================================================================================
		// The type of view, which determines how it is processed by the render pipeline.
		ERenderViewType Type = ERenderViewType::None;

		//=========================================================================================
		// Whether this view is enabled and should be rendered.
		bool Enabled = true;

		//=========================================================================================
		// The render target texture where the view will be rendered.
		RHITextureRef OutputTexture = nullptr;

		//=========================================================================================
		// Additional data depending on the view type.
		union
		{
			//=====================================================================================
			struct
			{
				//=================================================================================
				// The ID of the light that this shadow map is associated with.
				LightID Light = -1;

				//=================================================================================
				// The type of light casting the shadows (e.g., directional, point, spot).
				ELightType LightType = ELightType::Unknown;
			} Shadow;

			//=====================================================================================
			struct
			{
				//=================================================================================
				// A custom identifier that can be used to differentiate between multiple custom views.
				uint32_t ID = 0;

				//=================================================================================
				// User-defined data associated with the custom view.
				void* UserData = nullptr;
			} Custom;
		};
	};

} // namespace Tridium