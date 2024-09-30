#include "tripch.h"
#include "CameraComponent.h"
#include <Tridium/ECS/Components/Types/Common/TransformComponent.h>

namespace Tridium {
	BEGIN_REFLECT_COMPONENT( CameraComponent )
		BASE( Component )
		PROPERTY( SceneCamera, FLAGS(Serialize, EditAnywhere) )
	END_REFLECT( CameraComponent )

	Matrix4 CameraComponent::GetView()
	{
		auto& transform = GetGameObject().GetComponent<TransformComponent>();
		auto orientation = Quaternion( Vector3( -transform.Rotation.x, -transform.Rotation.y, 0.f ) );
		Matrix4 viewMatrix = glm::translate( Matrix4( 1.f ), transform.Position ) * glm::toMat4( orientation );
		return glm::inverse( viewMatrix );
	}
}
