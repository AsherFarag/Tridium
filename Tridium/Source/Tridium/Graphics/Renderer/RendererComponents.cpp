#include "tripch.h"
#include "RendererComponents.h"
#include <Tridium/Reflection/RuntimeMeta.h>

namespace Tridium {

	REGISTER_TYPE( CameraComponent );
	REGISTER_TYPE( CameraComponent::EProjectionType );
	REGISTER_TYPE( CameraComponent::PerspectiveData );
	REGISTER_TYPE( CameraComponent::OrthographicData );
	REGISTER_TYPE( StaticMeshComponent );
	REGISTER_TYPE( DirectionalLightComponent );
	REGISTER_TYPE( PointLightComponent );
	REGISTER_TYPE( SpotLightComponent );
	REGISTER_TYPE( SkyboxComponent );

	Matrix4 CameraComponent::CalculatePerspective() const
	{
		const float aspectRatio = AspectRatio();
		return Math::Perspective(
				glm::radians( Perspective.FOV ),
				aspectRatio,
				Perspective.NearPlane,
				Perspective.FarPlane );
	}

	Matrix4 CameraComponent::CalculateOrthographic() const
	{
		const float aspectRatio = AspectRatio();
		float orthoLeft = -Orthographic.Size * aspectRatio * 0.5f;
		float orthoRight = Orthographic.Size * aspectRatio * 0.5f;
		float orthoBottom = -Orthographic.Size * 0.5f;
		float orthoTop = Orthographic.Size * 0.5f;

		return Math::Orthographic( orthoLeft, orthoRight,
			orthoBottom, orthoTop, Orthographic.NearPlane, Orthographic.FarPlane );
	}

} // namespace Tridium