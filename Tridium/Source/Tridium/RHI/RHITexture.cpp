#include "tripch.h"
#include "RHITexture.h"

// Backends
#include "Backend/OpenGL/OpenGLTexture.h"
#include "Backend/DirectX12/DX12Texture.h"

namespace Tridium {

	SharedPtr<RHITexture> RHITexture::Create( const RHITextureDescriptor& a_Desc )
	{
		switch ( RHI::GetGraphicsAPI() )
		{
			case EGraphicsAPI::OpenGL:
			{
				return MakeShared<GL::OpenGLTexture>();
			}
			case EGraphicsAPI::DirectX12:
			{
				return MakeShared<DX12::DX12Texture>();
			}
			default:
			{
				ASSERT( false );
				return nullptr;
			}
		}
	}

} // namespace Tridium