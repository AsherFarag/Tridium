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
		const Vector3 normal = glm::normalize( a_Forward );
		Frustum frustum;
		const float halfVSide = m_Perspective.Far * glm::tan( glm::radians( m_Perspective.FOV ) * 0.5f );
		const float halfHSide = halfVSide * GetAspectRatio();
		const Vector3 frontMultFar = normal * m_Perspective.Far;
		const Vector3 frontMultNear = normal * m_Perspective.Near;

		const Vector3 right = glm::normalize( glm::cross( normal, { 0.0f, 1.0f, 0.0f } ) );
		const Vector3 up = glm::normalize( glm::cross( right, normal ) );

		static const auto CreatePlane = +[]( const Vector3& a_Normal, const Vector3& a_Position ) -> Plane
			{
				return { a_Normal, glm::dot( a_Normal, a_Position ) };
			};

		frustum.Near = CreatePlane( normal, a_Position + frontMultNear );
		frustum.Far = CreatePlane( -normal, a_Position + frontMultFar);
		frustum.Right = CreatePlane( glm::cross( frontMultFar - right * halfHSide, up ), a_Position );
		frustum.Left = CreatePlane( glm::cross( up, frontMultFar + right * halfHSide ), a_Position );
		frustum.Top = CreatePlane( glm::cross( right, frontMultFar + up * halfVSide ), a_Position );
		frustum.Bottom = CreatePlane( glm::cross( frontMultFar - up * halfVSide, right ), a_Position );

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