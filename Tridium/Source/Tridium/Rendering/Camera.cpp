#include "tripch.h"
#include "Camera.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	Camera::Camera()
	{
		RecalculateProjection();
	}

	Frustum Camera::GetFrustum( const Vector3& a_Position, Vector3 a_Forward ) const
	{
		switch ( m_ProjectionType )
		{
		case EProjectionType::Perspective:
			return GetPerspectiveFrustum( a_Position, a_Forward );
		case EProjectionType::Orthographic:
			return GetOrthographicFrustum( a_Position, a_Forward );
		}

		TE_CORE_ASSERT( false, "Invalid Projection Type!" );
		return Frustum();
	}

	Frustum Camera::GetPerspectiveFrustum( const Vector3& a_Position, Vector3 a_Forward ) const
	{
		// Ensure forward is normalized
		a_Forward = glm::normalize( a_Forward );
		Frustum frustum;

		// Precompute necessary values
		const float aspect = GetAspectRatio();
		const float halfVSide = m_Perspective.Far * glm::tan( glm::radians( m_Perspective.FOV ) * 0.5f );
		const float halfHSide = halfVSide * aspect;
		const Vector3 frontMultFar = a_Forward * m_Perspective.Far;

		// Right and up vectors for camera basis
		const Vector3 up = glm::normalize( Vector3( 0.0f, 1.0f, 0.0f ) );
		const Vector3 right = glm::normalize( glm::cross( a_Forward, up ) );

		// Near plane
		frustum.Near.Normal = -a_Forward;
		frustum.Near.Distance = glm::dot( a_Position + a_Forward * m_Perspective.Near, frustum.Near.Normal );

		// Far plane
		frustum.Far.Normal = a_Forward;
		frustum.Far.Distance = glm::dot( a_Position + frontMultFar, frustum.Far.Normal );

		// Top plane
		const Vector3 topNormal = glm::normalize( glm::cross( glm::normalize( right + up * halfVSide ), a_Forward ) );
		frustum.Top.Normal = topNormal;
		frustum.Top.Distance = glm::dot( a_Position, topNormal );

		// Bottom plane
		const Vector3 bottomNormal = glm::normalize( glm::cross( glm::normalize( right - up * halfVSide ), a_Forward ) );
		frustum.Bottom.Normal = bottomNormal;
		frustum.Bottom.Distance = glm::dot( a_Position, bottomNormal );

		// Right plane
		const Vector3 rightNormal = glm::normalize( glm::cross( glm::normalize( up - right * halfHSide ), a_Forward ) );
		frustum.Right.Normal = rightNormal;
		frustum.Right.Distance = glm::dot( a_Position, rightNormal );

		// Left plane
		const Vector3 leftNormal = glm::normalize( glm::cross( glm::normalize( up + right * halfHSide ), a_Forward ) );
		frustum.Left.Normal = leftNormal;
		frustum.Left.Distance = glm::dot( a_Position, leftNormal );

		return frustum;
	}


	Frustum Camera::GetOrthographicFrustum( const Vector3& a_Position, Vector3 a_Forward ) const
	{
		TE_CORE_ASSERT( false, "Orthographic Frustum not implemented!" );
		return Frustum();
	}

	void Camera::SetPerspective()
	{
		m_ProjectionType = EProjectionType::Perspective;
		RecalculateProjection();
	}

	void Camera::SetOrthographic()
	{
		m_ProjectionType = EProjectionType::Orthographic;
		RecalculateProjection();
	}

	void Camera::SetPerspective( float verticalFOV, float nearClip, float farClip )
	{
		m_Perspective.FOV = verticalFOV;
		m_Perspective.Near = nearClip;
		m_Perspective.Far = farClip;
		SetPerspective();
	}

	void Camera::SetOrthographic( float size, float nearClip, float farClip )
	{
		m_Orthographic.Size = size;
		m_Orthographic.Near = nearClip;
		m_Orthographic.Far = farClip;
		SetOrthographic();
	}

	void Camera::SetViewportSize( const iVector2& a_Size )
	{
		TE_CORE_ASSERT( a_Size.x > 0 && a_Size.y > 0, "Invalid Viewport width and height!" );
		m_ViewportSize = a_Size;
		RecalculateProjection();
	}

	void Camera::RecalculateProjection()
	{
		if ( m_ProjectionType == EProjectionType::Perspective )
		{
			m_Projection = glm::perspective( glm::radians( m_Perspective.FOV ), GetAspectRatio(), m_Perspective.Near, m_Perspective.Far);
		}
		else // Orthographic
		{
			float orthoLeft = -m_Orthographic.Size * GetAspectRatio() * 0.5f;
			float orthoRight = m_Orthographic.Size * GetAspectRatio() * 0.5f;
			float orthoBottom = -m_Orthographic.Size * 0.5f;
			float orthoTop = m_Orthographic.Size * 0.5f;

			m_Projection = glm::ortho( orthoLeft, orthoRight,
				orthoBottom, orthoTop, m_Orthographic.Near, m_Orthographic.Far );
		}

	}

	BEGIN_REFLECT_ENUM( EProjectionType, Scriptable )
		ENUM_VALUE( Perspective )
		ENUM_VALUE( Orthographic )
	END_REFLECT_ENUM( EProjectionType )

	BEGIN_REFLECT( PerspectiveData, Scriptable )
		PROPERTY( FOV, Serialize | EditAnywhere )
		PROPERTY( Near, Serialize | EditAnywhere )
		PROPERTY( Far, Serialize | EditAnywhere )
	END_REFLECT( PerspectiveData );

	BEGIN_REFLECT( OrthographicData, Scriptable )
		PROPERTY( Size, Serialize | EditAnywhere )
		PROPERTY( Near, Serialize | EditAnywhere )
		PROPERTY( Far, Serialize | EditAnywhere )
	END_REFLECT( OrthographicData )

	BEGIN_REFLECT( Camera, Scriptable )
		PROPERTY( m_Perspective, Serialize | EditAnywhere )
		PROPERTY( m_Orthographic, Serialize | EditAnywhere )
		PROPERTY( m_ProjectionType, Serialize | EditAnywhere )
		PROPERTY( m_Projection, Serialize | EditAnywhere )
		PROPERTY( m_ViewportSize, Serialize | EditAnywhere )
	END_REFLECT( Camera )

} // namespace Tridium