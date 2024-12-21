#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/Math/AABB.h>
#include <Tridium/Debug/DebugColors.h>

// Forward Declarations
namespace Tridium {
	class Shader;
} // namespace Tridium

namespace Tridium::Debug {

	enum class EDrawDuration
	{
		Permanent = 0,
		OneFrame = 1,
		ForDuration = 2
	};

	struct DrawSettings
	{
		Color Color;
		EDrawDuration DrawDuration;
		float Duration;
	};

} // namespace Tridium::Debug

#if TE_USE_DEBUG

#include <array>
#include <vector>
#include <queue>

namespace Tridium::Debug {

	class DebugDrawer final
	{
	public:
		static DebugDrawer& Get() { return *s_Instance; }
		void Draw( const Matrix4& a_ViewProjection );

		void DrawLine( const Vector3& a_Start, const Vector3& a_End, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f );
		void DrawTriangle( const Vector3& a_A, const Vector3& a_B, const Vector3& a_C, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f );
		void DrawAABB( const AABB& a_AABB, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f );
		void DrawAABBFilled( const AABB& a_AABB, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f );
		void DrawSphere( const Vector3& a_Center, float a_Radius, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f );
		void DrawSphereFilled( const Vector3& a_Center, float a_Radius, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f );
		void DrawBox( const Vector3& a_Center, const Quaternion& a_Rotation, const Vector3& a_HalfExtents, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f );
		void DrawBoxFilled( const Vector3& a_Center, const Quaternion& a_Rotation, const Vector3& a_HalfExtents, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f );
		void DrawFrustum( const Matrix4& a_ViewProjection, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f );

	private:
		DebugDrawer();
		static void Init();

		static UniquePtr<DebugDrawer> s_Instance;

	private:
		template<typename T>
		struct ShapeList
		{
			std::vector<T> Permenant;
			std::vector<T> OneFrame;
			std::deque<std::pair<float, T>> ForDuration;
		};

		struct DebugVertex
		{
			Vector3 Position;
			Color Color;
		};

		struct DebugLine
		{
			DebugVertex Start;
			DebugVertex End;
		};

		struct DebugTri
		{
			DebugVertex A;
			DebugVertex B;
			DebugVertex C;
		};

		ShapeList<DebugLine> m_Lines;
		ShapeList<DebugTri> m_Tris;

		SharedPtr<Shader> m_Shader;
		uint32_t m_LineVAO;
		uint32_t m_LineVBO;
		uint32_t m_TriVAO;
		uint32_t m_TriVBO;

		friend class Application;
	};

	static inline void DrawLine( const Vector3& a_Start, const Vector3& a_End, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f )
	{
		DebugDrawer::Get().DrawLine( a_Start, a_End, a_Color, a_DrawDuration, a_Duration );
	}

	static inline void DrawTriangle( const Vector3& a_A, const Vector3& a_B, const Vector3& a_C, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f )
	{
		DebugDrawer::Get().DrawTriangle( a_A, a_B, a_C, a_Color, a_DrawDuration, a_Duration );
	}

	static inline void DrawAABB( const AABB& a_AABB, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f )
	{
		DebugDrawer::Get().DrawAABB( a_AABB, a_Color, a_DrawDuration, a_Duration );
	}

	static inline void DrawAABBFilled( const AABB& a_AABB, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f )
	{
		DebugDrawer::Get().DrawAABBFilled( a_AABB, a_Color, a_DrawDuration, a_Duration );
	}

	static inline void DrawSphere( const Vector3& a_Center, float a_Radius, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f )
	{
		DebugDrawer::Get().DrawSphere( a_Center, a_Radius, a_Color, a_DrawDuration, a_Duration );
	}

	static inline void DrawSphereFilled( const Vector3& a_Center, float a_Radius, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f )
	{
		DebugDrawer::Get().DrawSphereFilled( a_Center, a_Radius, a_Color, a_DrawDuration, a_Duration );
	}

	static inline void DrawBox( const Vector3& a_Center, const Quaternion& a_Rotation, const Vector3& a_HalfExtents, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f )
	{
		DebugDrawer::Get().DrawBox( a_Center, a_Rotation, a_HalfExtents, a_Color, a_DrawDuration, a_Duration );
	}

	static inline void DrawBoxFilled( const Vector3& a_Center, const Quaternion& a_Rotation, const Vector3& a_HalfExtents, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f )
	{
		DebugDrawer::Get().DrawBoxFilled( a_Center, a_Rotation, a_HalfExtents, a_Color, a_DrawDuration, a_Duration );
	}

	static inline void DrawFrustum( const Matrix4& a_ViewProjection, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f )
	{
		DebugDrawer::Get().DrawFrustum( a_ViewProjection, a_Color, a_DrawDuration, a_Duration );
	}

} // namespace Tridium::Debug

#else

namespace Tridium::Debug {

	static inline void DrawLine( const Vector3& a_Start, const Vector3& a_End, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f ) {}
	static inline void DrawTriangle( const Vector3& a_A, const Vector3& a_B, const Vector3& a_C, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f ) {}
	static inline void DrawAABB( const AABB& a_AABB, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f ) {}
	static inline void DrawAABBFilled( const AABB& a_AABB, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f ) {}
	static inline void DrawSphere( const Vector3& a_Center, float a_Radius, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f ) {}
	static inline void DrawSphereFilled( const Vector3& a_Center, float a_Radius, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f ) {}
	static inline void DrawBox( const Vector3& a_Center, const Quaternion& a_Rotation, const Vector3& a_HalfExtents, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f ) {}
	static inline void DrawBoxFilled( const Vector3& a_Center, const Quaternion& a_Rotation, const Vector3& a_HalfExtents, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f ) {}
	static inline void DrawFrustum( const Matrix4& a_ViewProjection, const Color& a_Color, EDrawDuration a_DrawDuration = EDrawDuration::OneFrame, float a_Duration = 0.0f ) {}

} // namespace Tridium::Debug

#endif // TE_USE_DEBUG