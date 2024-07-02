#include "tripch.h"

#ifdef IS_EDITOR
#include "ViewportPanel.h"

namespace Tridium::Editor {

	ViewportPanel::ViewportPanel( const std::string& name )
		: Panel( name )
	{
		FramebufferSpecification FBOspecification;
		FBOspecification.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		FBOspecification.Width = 1280;
		FBOspecification.Height = 720;
		m_FBO = Framebuffer::Create( FBOspecification );
	}
}


#endif // IS_EDITOR
