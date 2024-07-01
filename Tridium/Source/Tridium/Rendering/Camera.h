#pragma once

namespace Tridium {

	class Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	public:
		Camera();
		~Camera() = default;

		const Matrix4& GetProjection() const { return m_Projection; }

		void SetPerspective();
		void SetOrthographic();
		void SetPerspective( float FOV, float nearClip, float farClip );
		void SetOrthographic( float size, float nearClip, float farClip );

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetViewportSize( uint32_t width, uint32_t height );

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

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType( ProjectionType type ) { m_ProjectionType = type; RecalculateProjection(); }

	private:
		void RecalculateProjection();

	private:
		ProjectionType m_ProjectionType = ProjectionType::Perspective;
		Matrix4 m_Projection = Matrix4( 1.0f );

		struct PerspectiveData
		{
			float FOV = 45.0f;
			float Near = 0.01f, Far = 1000.0f;
		} m_Perspective;

		struct OrthographicData
		{
			float Size = 10.0f;
			float Near = -1000.0f, Far = 1000.0f;
		} m_Orthographic;


		float m_AspectRatio = 16.f / 9.0f;
	};

}