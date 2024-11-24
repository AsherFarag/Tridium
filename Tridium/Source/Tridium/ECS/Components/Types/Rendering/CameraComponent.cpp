#include "tripch.h"
#include "CameraComponent.h"
#include <Tridium/ECS/Components/Types/Common/TransformComponent.h>

namespace Tridium {

	CameraComponent::~CameraComponent()
	{
		TE_CORE_INFO( "CameraComponent Destroyed" );
	}

	Matrix4 CameraComponent::GetView()
	{
		auto& transform = GetGameObject().GetComponent<TransformComponent>();
		auto orientation = transform.Rotation.Quat * Quaternion( Vector3( glm::radians( -Pitch ), glm::radians( -Yaw ), 0.f ) );
		Matrix4 viewMatrix = glm::translate( Matrix4( 1.f ), transform.Position ) * glm::toMat4( orientation );
		return glm::inverse( viewMatrix );
	}
}
