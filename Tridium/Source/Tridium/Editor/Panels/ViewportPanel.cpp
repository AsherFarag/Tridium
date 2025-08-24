#include "tripch.h"

#if IS_EDITOR
#include "ViewportPanel.h"
#include <Tridium/Graphics/RHI/RHI.h>

namespace Tridium {

	ViewportPanel::ViewportPanel( const String& a_Name )
		: Panel( a_Name )
	{
		const auto colorAttachmentDesc = RHITextureDesc{}
			.SetFormat( ERHIFormat::RGBA8_UINT )
			.SetDimension( ERHITextureDimension::Texture2D )
			.SetWidth( 1280 )
			.SetHeight( 720 )
			.SetBindFlags( ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource )
			.SetName( "Viewport Color Attachment 1" );

		const auto depthAttachmentDesc = RHITextureDesc{}
			.SetFormat( ERHIFormat::D24_UNORM_S8_UINT )
			.SetDimension( ERHITextureDimension::Texture2D )
			.SetWidth( 1280 )
			.SetHeight( 720 )
			.SetBindFlags( ERHIBindFlags::DepthStencil | ERHIBindFlags::ShaderResource )
			.SetName( "Viewport Depth Attachment" );

		m_FBO.AddColorAttachment( RHI::CreateTexture( colorAttachmentDesc ) );
		m_FBO.SetDepthStencilAttachment( RHI::CreateTexture( depthAttachmentDesc ) );
	}

}


#endif // IS_EDITOR
