#include "tripch.h"
#include "TransformComponent.h"

namespace Tridium {

	TransformComponent::TransformComponent( const Vector3& a_Translation )
		: Translation( a_Translation ) {}

	Matrix4 TransformComponent::GetTransform() const
	{
		Matrix4 rotationMatrix = glm::toMat4( Quaternion( Rotation ) );

		constexpr Matrix4 identity = Matrix4( 1.0f );

		return glm::translate( identity, Translation )
			* rotationMatrix
			* glm::scale( identity, Scale );
	}

	Vector3 TransformComponent::GetForward() const
	{
		const Matrix4 inverted = glm::inverse( GetTransform() );
		return normalize( glm::vec3( inverted[ 0 ] ) ); // Forward Vector
	}

}