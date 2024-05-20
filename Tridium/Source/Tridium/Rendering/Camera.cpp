#include "tripch.h"
#include "Camera.h"

namespace Tridium {

	Camera::Camera()
	{
		RecalculateProjection();
	}

	void Camera::SetPerspective()
	{
		m_ProjectionType = ProjectionType::Perspective;
		RecalculateProjection();
	}

	void Camera::SetOrthographic()
	{
		m_ProjectionType = ProjectionType::Orthographic;
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

	void Camera::SetViewportSize( uint32_t width, uint32_t height )
	{
		TE_CORE_ASSERT( width > 0 && height > 0, "Invalid Viewport width and height!" );
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void Camera::RecalculateProjection()
	{
		if ( m_ProjectionType == ProjectionType::Perspective )
		{
			m_Projection = glm::perspective( m_Perspective.FOV, m_AspectRatio, m_Perspective.Near, m_Perspective.Far );
		}
		else // Orthographic
		{
			float orthoLeft = -m_Orthographic.Size * m_AspectRatio * 0.5f;
			float orthoRight = m_Orthographic.Size * m_AspectRatio * 0.5f;
			float orthoBottom = -m_Orthographic.Size * 0.5f;
			float orthoTop = m_Orthographic.Size * 0.5f;

			m_Projection = glm::ortho( orthoLeft, orthoRight,
				orthoBottom, orthoTop, m_Orthographic.Near, m_Orthographic.Far );
		}

	}


}