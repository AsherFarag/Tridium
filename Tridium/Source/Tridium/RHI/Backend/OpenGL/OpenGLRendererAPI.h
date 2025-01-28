#pragma once
#include <Tridium/RHI/RendererAPI.h>

namespace Tridium {

	class OpenGLRendererAPI final : public RendererAPI
	{
	public:
		virtual bool Init( const RHIConfig& a_Config ) override;
		virtual bool Shutdown() override;
		virtual bool Present() override;
	};

}