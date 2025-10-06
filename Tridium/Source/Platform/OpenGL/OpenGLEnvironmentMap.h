#pragma once
#include <Tridium/Graphics/oldRendering/EnvironmentMap.h>

namespace Tridium {

    class OpenGLEnvironmentMap : public EnvironmentMapOld
    {
	public:
		OpenGLEnvironmentMap( const SharedPtr<TextureOld>& a_EquirectangularTexture );

    protected:
		void PerformDiffuseConvolution( SharedPtr<CubeMap> a_EnvironmentCubeMap );
		void PerformSpecularConvolution( SharedPtr<CubeMap> a_EnvironmentCubeMap );
    };

}