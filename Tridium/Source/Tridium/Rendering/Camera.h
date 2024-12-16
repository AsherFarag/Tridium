#pragma once
#include <Tridium/Reflection/ReflectionFwd.h>
#include "Frustum.h"

namespace Tridium {

	struct PerspectiveData
	{
		REFLECT( PerspectiveData );
		float FOV = 45.0f;
		float Near = 0.01f, Far = 1000.0f;
	};

	struct OrthographicData
	{
		REFLECT( OrthographicData );
		float Size = 10.0f;
		float Near = -1000.0f, Far = 1000.0f;
	};

	enum class EProjectionType { Perspective = 0, Orthographic = 1 };

	class Camera
	{
		REFLECT( Camera );

	public:
		Camera();
		~Camera() = default;

		const Matrix4& GetProjection() const { return m_Projection; }
		Frustum GetFrustum( const Vector3& a_Position, Vector3 a_Forward ) const;
		Frustum GetPerspectiveFrustum( const Vector3& a_Position, Vector3 a_Forward ) const;
		Frustum GetOrthographicFrustum( const Vector3& a_Position, Vector3 a_Forward ) const;

		void SetPerspective();
		void SetOrthographic();
		void SetPerspective( float FOV, float nearClip, float farClip );
		void SetOrthographic( float size, float nearClip, float farClip );

		float GetAspectRatio() const { return m_ViewportSize.x / m_ViewportSize.y; }
		const iVector2& GetViewportSize() const { return m_ViewportSize; }
		void SetViewportSize( const iVector2& a_Size );
		void SetViewportSize( uint32_t a_Width, uint32_t a_Heigh ) { SetViewportSize( iVector2( a_Width, a_Heigh ) ); }

		float GetPerspectiveFOV() const { return m_Perspective.FOV; } 
		void SetPerspectiveFOV( float FOV ) { m_Perspective.FOV = FOV; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_Perspective.Near; }
		void SetPerspectiveNearClip( float nearClip ) { m_Perspective.Near = nearClip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_Perspective.Far; }
		void SetPerspectiveFarClip( float farClip ) { m_Perspective.Far = farClip; RecalculateProjection(); }

		float GetOrthographicSize() const { return m_Orthographic.Size; }
		void  SetOrthographicSize( float size ) { m_Orthographic.Size = size; RecalculateProjection(); }
		float GetOrthographicNearClip() const { return m_Orthographic.Near; }
		void  SetOrthographicNearClip( float nearClip ) { m_Orthographic.Near = nearClip; RecalculateProjection(); }
		float GetOrthographicFarClip() const { return m_Orthographic.Far; }
		void  SetOrthographicFarClip( float farClip ) { m_Orthographic.Far = farClip; RecalculateProjection(); }

		EProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType( EProjectionType type ) { m_ProjectionType = type; RecalculateProjection(); }

	private:
		void RecalculateProjection();

	private:
		EProjectionType m_ProjectionType = EProjectionType::Perspective;
		Matrix4 m_Projection = Matrix4( 1.0f );

		PerspectiveData m_Perspective;
		OrthographicData m_Orthographic;

		Vector2 m_ViewportSize = { 1280, 720 };
	};
}