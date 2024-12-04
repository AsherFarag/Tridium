#include "tripch.h"
#include "CameraComponent.h"
#include <Tridium/ECS/Components/Types/Common/TransformComponent.h>
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	CameraComponent::~CameraComponent()
	{
	}

	Matrix4 CameraComponent::GetView()
	{
		TransformComponent& transform = GetGameObject().GetTransform();

		// Step 1: Convert quaternion to rotation matrix
		Matrix4 rotationMatrix = glm::toMat4( transform.GetOrientation() );

		// Step 2: Create a translation matrix for the camera's position
		Matrix4 translationMatrix = glm::translate( Matrix4( 1.0f ), transform.GetWorldPosition() );

		// Step 3: Combine the translation and rotation (order matters)
		return glm::inverse( translationMatrix * rotationMatrix );
	}


	BEGIN_REFLECT_COMPONENT( CameraComponent, Scriptable )
		PROPERTY( SceneCamera, Serialize | EditAnywhere )
		PROPERTY( IsMainCamera, Serialize | EditAnywhere )
	END_REFLECT_COMPONENT( CameraComponent )
}
