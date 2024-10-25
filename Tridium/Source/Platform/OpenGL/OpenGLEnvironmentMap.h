#pragma once
#include <Tridium/Rendering/EnvironmentMap.h>

namespace Tridium {

    class OpenGLEnvironmentMap : public EnvironmentMap
    {
	public:
		OpenGLEnvironmentMap( const SharedPtr<Texture>& a_EquirectangularTexture );

    protected:
		void PerformDiffuseConvolution( SharedPtr<CubeMap> a_EnvironmentCubeMap );
		void PerformSpecularConvolution( SharedPtr<CubeMap> a_EnvironmentCubeMap );
    };

}