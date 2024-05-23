#include "tripch.h"
#include "TransformComponent.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace Tridium {

	TransformComponent::TransformComponent( const Vector3& a_Translation )
		: Position( a_Translation ) {}

	Matrix4 TransformComponent::GetTransform() const
	{
		Matrix4 rotationMatrix = glm::toMat4( Quaternion( Rotation ) );

		constexpr Matrix4 identity = Matrix4( 1.0f );

		return glm::translate( identity, Position )
			* rotationMatrix
			* glm::scale( identity, Scale );
	}

	Vector3 TransformComponent::GetForward() const
	{
		return glm::rotate( GetOrientation(), Vector3( 0.0f, 0.0f, -1.0f ) );
	}

	Quaternion TransformComponent::GetOrientation() const
	{
		return Quaternion( Vector3( -Rotation.x, -Rotation.y, 0.f ) );
	}

}