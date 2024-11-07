#include "tripch.h"
#include "CameraComponent.h"
#include <Tridium/ECS/Components/Types/Common/TransformComponent.h>

namespace Tridium {

	Matrix4 CameraComponent::GetView()
	{
		auto& transform = GetGameObject().GetComponent<TransformComponent>();
		auto orientation = transform.Rotation.Quat;
		Matrix4 viewMatrix = glm::translate( Matrix4( 1.f ), transform.Position ) * glm::toMat4( orientation );
		return glm::inverse( viewMatrix );
	}
}
