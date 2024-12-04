#include "tripch.h"
#include "Camera.h"
#include <Tridium/Reflection/Reflection.h>

namespace Tridium {

	Camera::Camera()
	{
		RecalculateProjection();
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