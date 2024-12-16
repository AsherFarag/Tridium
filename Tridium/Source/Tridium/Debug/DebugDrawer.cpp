#include "tripch.h"
#if TE_USE_DEBUG

#include "DebugDrawer.h"
#include <Tridium/Rendering/VertexArray.h>
#include <Tridium/Rendering/Shader.h>

// TEMP!
#include <glad/glad.h>

namespace Tridium::Debug { 

	UniquePtr<DebugDrawer> DebugDrawer::s_Instance = nullptr;

	void DebugDrawer::DrawLine( const Vector3& a_Start, const Vector3& a_End, const Color& a_Color, EDrawDuration a_DrawDuration, float a_Duration )
	{
		DebugLine line = { { a_Start, a_Color }, { a_End, a_Color } };
		switch ( a_DrawDuration )
		{
		case EDrawDuration::Permanent:
		{
			m_Lines.Permenant.push_back( line );
			break;
		}
		case EDrawDuration::OneFrame:
		{
			m_Lines.OneFrame.push_back( line );
			break;
		}
		case EDrawDuration::ForDuration:
		{
			float deathTime = a_Duration + Time::Now();
			m_Lines.ForDuration.push_back( { deathTime, line } );
			break;
		}
		default:
		{
			TE_CORE_ASSERT( false, "Invalid EDrawDuration" );
			break;
		}
		}
	}

	void DebugDrawer::DrawTriangle( const Vector3& a_A, const Vector3& a_B, const Vector3& a_C, const Color& a_Color, EDrawDuration a_DrawDuration, float a_Duration )
	{
		DebugTri tri = { { a_A, a_Color }, { a_B, a_Color }, { a_C, a_Color } };
		switch ( a_DrawDuration )
		{
		case EDrawDuration::Permanent:
		{
			m_Tris.Permenant.push_back( tri );
			break;
		}
		case EDrawDuration::OneFrame:
		{
			m_Tris.OneFrame.push_back( tri );
			break;
		}
		case EDrawDuration::ForDuration:
		{
			float deathTime = a_Duration + Time::Now();
			m_Tris.ForDuration.push_back( { deathTime, tri } );
			break;
		}
		default:
		{
			TE_CORE_ASSERT( false, "Invalid EDrawDuration" );
			break;
		}
		}
	}

	void DebugDrawer::DrawAABB( const AABB& a_AABB, const Color& a_Color, EDrawDuration a_DrawDuration, float a_Duration )
	{
		Vector3 min = a_AABB.Min;
		Vector3 max = a_AABB.Max;

		// Bottom face
		// b2 - b1
		// |     |
		// b4 - b3
		Vector3 b1 = { min.x, min.y, min.z };
		Vector3 b2 = { max.x, min.y, min.z };
		Vector3 b3 = { min.x, min.y, max.z };
		Vector3 b4 = { max.x, min.y, max.z };

		// Top face
		// t2 - t1
		// |     |
		// t4 - t3
		Vector3 t1 = { max.x, max.y, max.z };
		Vector3 t2 = { min.x, max.y, max.z };
		Vector3 t3 = { max.x, max.y, min.z };
		Vector3 t4 = { min.x, max.y, min.z };

		// Draw Top face
		DrawLine( t1, t2, a_Color, a_DrawDuration, a_Duration );
		DrawLine( t2, t4, a_Color, a_DrawDuration, a_Duration );
		DrawLine( t4, t3, a_Color, a_DrawDuration, a_Duration );
		DrawLine( t3, t1, a_Color, a_DrawDuration, a_Duration );

		// Draw Bottom face
		DrawLine( b1, b2, a_Color, a_DrawDuration, a_Duration );
		DrawLine( b2, b4, a_Color, a_DrawDuration, a_Duration );
		DrawLine( b4, b3, a_Color, a_DrawDuration, a_Duration );
		DrawLine( b3, b1, a_Color, a_DrawDuration, a_Duration );

		// Draw Side faces
		DrawLine( b1, t4, a_Color, a_DrawDuration, a_Duration );
		DrawLine( b2, t3, a_Color, a_DrawDuration, a_Duration );
		DrawLine( b3, t2, a_Color, a_DrawDuration, a_Duration );
		DrawLine( b4, t1, a_Color, a_DrawDuration, a_Duration );
	}

	void DebugDrawer::DrawAABBFilled( const AABB& a_AABB, const Color& a_Color, EDrawDuration a_DrawDuration, float a_Duration )
	{
		Vector3 min = a_AABB.Min;
		Vector3 max = a_AABB.Max;

		// Bottom face
		// b2 - b1
		// |     |
		// b4 - b3
		Vector3 b1 = { min.x, min.y, min.z };
		Vector3 b2 = { max.x, min.y, min.z };
		Vector3 b3 = { min.x, min.y, max.z };
		Vector3 b4 = { max.x, min.y, max.z };

		// Top face
		// t2 - t1
		// |     |
		// t4 - t3

		Vector3 t4 = { max.x, max.y, max.z };
		Vector3 t3 = { min.x, max.y, max.z };
		Vector3 t2 = { max.x, max.y, min.z };
		Vector3 t1 = { min.x, max.y, min.z };

		// Draw Top face
		DrawTriangle( t1, t2, t4, a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( t1, t4, t3, a_Color, a_DrawDuration, a_Duration );

		// Draw Bottom face
		DrawTriangle( b1, b2, b4, a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( b1, b4, b3, a_Color, a_DrawDuration, a_Duration );

		// Draw Front face
		DrawTriangle( b1, t2, t1, a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( b1, b2, t2, a_Color, a_DrawDuration, a_Duration );

		// Draw Back face
		DrawTriangle( b3, t4, t3, a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( b3, b4, t4, a_Color, a_DrawDuration, a_Duration );

		// Draw Left face
		DrawTriangle( b1, t1, t3, a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( b1, b3, t3, a_Color, a_DrawDuration, a_Duration );

		// Draw Right face
		DrawTriangle( b2, t2, t4, a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( b2, b4, t4, a_Color, a_DrawDuration, a_Duration );

	}

	void DebugDrawer::DrawSphere( const Vector3& a_Center, float a_Radius, const Color& a_Color, EDrawDuration a_DrawDuration, float a_Duration )
	{
		constexpr int segments = 16;
		constexpr float angle = 2.0f * glm::pi<float>() / segments;

		Vector3 last = a_Center + Vector3{ a_Radius, 0.0f, 0.0f };
		for ( int i = 1; i <= segments; ++i ) 
		{
			float x = a_Radius * glm::cos( i * angle );
			float y = a_Radius * glm::sin( i * angle );
			Vector3 next = a_Center + Vector3{ x, y, 0.0f };
			DrawLine( last, next, a_Color, a_DrawDuration, a_Duration );
			last = next;
		}

		last = a_Center + Vector3{ 0.0f, a_Radius, 0.0f };
		for ( int i = 1; i <= segments; ++i )
		{
			float x = a_Radius * glm::cos( i * angle );
			float y = a_Radius * glm::sin( i * angle );
			Vector3 next = a_Center + Vector3{ x, 0.0f, y };
			DrawLine( last, next, a_Color, a_DrawDuration, a_Duration );
			last = next;
		}

		last = a_Center + Vector3{ 0.0f, 0.0f, a_Radius };
		for ( int i = 1; i <= segments; ++i )
		{
			float x = a_Radius * glm::cos( i * angle );
			float y = a_Radius * glm::sin( i * angle );
			Vector3 next = a_Center + Vector3{ 0.0f, x, y };
			DrawLine( last, next, a_Color, a_DrawDuration, a_Duration );
			last = next;
		}
	}

	void DebugDrawer::DrawSphereFilled( const Vector3& a_Center, float a_Radius, const Color& a_Color, EDrawDuration a_DrawDuration, float a_Duration )
	{
		constexpr int segments = 16;
		constexpr float angle = 2.0f * glm::pi<float>() / segments;

		std::array<Vector3, segments> top;
		std::array<Vector3, segments> bottom;
		std::array<Vector3, segments> middle;

		for ( int i = 0; i < segments; ++i )
		{
			float x = a_Radius * glm::cos( i * angle );
			float y = a_Radius * glm::sin( i * angle );
			top[i] = ( a_Center + Vector3{ x, y, 0.0f } );
			bottom[i] = ( a_Center + Vector3{ x, 0.0f, y } );
			middle[i] = ( a_Center + Vector3{ 0.0f, x, y } );
		}

		for ( int i = 0; i < segments; ++i )
		{
			DrawTriangle( a_Center, top[i], top[( i + 1 ) % segments], a_Color, a_DrawDuration, a_Duration );
			DrawTriangle( a_Center, bottom[i], bottom[( i + 1 ) % segments], a_Color, a_DrawDuration, a_Duration );
			DrawTriangle( a_Center, middle[i], middle[( i + 1 ) % segments], a_Color, a_DrawDuration, a_Duration );
		}

		for ( int i = 0; i < segments; ++i )
		{
			DrawTriangle( top[i], top[( i + 1 ) % segments], middle[i], a_Color, a_DrawDuration, a_Duration );
			DrawTriangle( top[( i + 1 ) % segments], middle[( i + 1 ) % segments], middle[i], a_Color, a_DrawDuration, a_Duration );

			DrawTriangle( bottom[i], bottom[( i + 1 ) % segments], middle[i], a_Color, a_DrawDuration, a_Duration );
			DrawTriangle( bottom[( i + 1 ) % segments], middle[( i + 1 ) % segments], middle[i], a_Color, a_DrawDuration, a_Duration );
		}
	}

	void DebugDrawer::DrawBox( const Vector3& a_Center, const Quaternion& a_Rotation, const Vector3& a_HalfExtents, const Color& a_Color, EDrawDuration a_DrawDuration, float a_Duration )
	{
		Vector3 corners[8];
		corners[0] = a_Center + a_Rotation * Vector3{ -a_HalfExtents.x, -a_HalfExtents.y, -a_HalfExtents.z };
		corners[1] = a_Center + a_Rotation * Vector3{ a_HalfExtents.x, -a_HalfExtents.y, -a_HalfExtents.z };
		corners[2] = a_Center + a_Rotation * Vector3{ a_HalfExtents.x, a_HalfExtents.y, -a_HalfExtents.z };
		corners[3] = a_Center + a_Rotation * Vector3{ -a_HalfExtents.x, a_HalfExtents.y, -a_HalfExtents.z };
		corners[4] = a_Center + a_Rotation * Vector3{ -a_HalfExtents.x, -a_HalfExtents.y, a_HalfExtents.z };
		corners[5] = a_Center + a_Rotation * Vector3{ a_HalfExtents.x, -a_HalfExtents.y, a_HalfExtents.z };
		corners[6] = a_Center + a_Rotation * Vector3{ a_HalfExtents.x, a_HalfExtents.y, a_HalfExtents.z };	
		corners[7] = a_Center + a_Rotation * Vector3{ -a_HalfExtents.x, a_HalfExtents.y, a_HalfExtents.z };

		DrawLine( corners[0], corners[1], a_Color, a_DrawDuration, a_Duration );
		DrawLine( corners[1], corners[2], a_Color, a_DrawDuration, a_Duration );
		DrawLine( corners[2], corners[3], a_Color, a_DrawDuration, a_Duration );
		DrawLine( corners[3], corners[0], a_Color, a_DrawDuration, a_Duration );

		DrawLine( corners[4], corners[5], a_Color, a_DrawDuration, a_Duration );
		DrawLine( corners[5], corners[6], a_Color, a_DrawDuration, a_Duration );
		DrawLine( corners[6], corners[7], a_Color, a_DrawDuration, a_Duration );
		DrawLine( corners[7], corners[4], a_Color, a_DrawDuration, a_Duration );

		DrawLine( corners[0], corners[4], a_Color, a_DrawDuration, a_Duration );
		DrawLine( corners[1], corners[5], a_Color, a_DrawDuration, a_Duration );
		DrawLine( corners[2], corners[6], a_Color, a_DrawDuration, a_Duration );
		DrawLine( corners[3], corners[7], a_Color, a_DrawDuration, a_Duration );
	}

	void DebugDrawer::DrawBoxFilled( const Vector3& a_Center, const Quaternion& a_Rotation, const Vector3& a_HalfExtents, const Color& a_Color, EDrawDuration a_DrawDuration, float a_Duration )
	{
		Vector3 corners[8];
		corners[0] = a_Center + a_Rotation * Vector3{ -a_HalfExtents.x, -a_HalfExtents.y, -a_HalfExtents.z };
		corners[1] = a_Center + a_Rotation * Vector3{ a_HalfExtents.x, -a_HalfExtents.y, -a_HalfExtents.z };
		corners[2] = a_Center + a_Rotation * Vector3{ a_HalfExtents.x, a_HalfExtents.y, -a_HalfExtents.z };
		corners[3] = a_Center + a_Rotation * Vector3{ -a_HalfExtents.x, a_HalfExtents.y, -a_HalfExtents.z };
		corners[4] = a_Center + a_Rotation * Vector3{ -a_HalfExtents.x, -a_HalfExtents.y, a_HalfExtents.z };
		corners[5] = a_Center + a_Rotation * Vector3{ a_HalfExtents.x, -a_HalfExtents.y, a_HalfExtents.z };
		corners[6] = a_Center + a_Rotation * Vector3{ a_HalfExtents.x, a_HalfExtents.y, a_HalfExtents.z };
		corners[7] = a_Center + a_Rotation * Vector3{ -a_HalfExtents.x, a_HalfExtents.y, a_HalfExtents.z };

		DrawTriangle( corners[0], corners[1], corners[2], a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( corners[0], corners[2], corners[3], a_Color, a_DrawDuration, a_Duration );

		DrawTriangle( corners[4], corners[7], corners[6], a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( corners[4], corners[6], corners[5], a_Color, a_DrawDuration, a_Duration );

		DrawTriangle( corners[0], corners[4], corners[5], a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( corners[0], corners[5], corners[1], a_Color, a_DrawDuration, a_Duration );

		DrawTriangle( corners[1], corners[5], corners[6], a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( corners[1], corners[6], corners[2], a_Color, a_DrawDuration, a_Duration );

		DrawTriangle( corners[2], corners[6], corners[7], a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( corners[2], corners[7], corners[3], a_Color, a_DrawDuration, a_Duration );

		DrawTriangle( corners[3], corners[7], corners[4], a_Color, a_DrawDuration, a_Duration );
		DrawTriangle( corners[3], corners[4], corners[0], a_Color, a_DrawDuration, a_Duration );
	}

	void DebugDrawer::DrawFrustum( const Matrix4& a_Projection, const Matrix4& a_View, const Color& a_Color, EDrawDuration a_DrawDuration, float a_Duration )
	{

	}

	DebugDrawer::DebugDrawer()
	{
		// Reserve some space for the vectors
		m_Lines.Permenant.reserve( 1024 );
		m_Lines.OneFrame.reserve( 1024 );
		m_Tris.Permenant.reserve( 1024 );
		m_Tris.OneFrame.reserve( 1024 );

		// Create Shader
		{
			std::string vertexSrc = R"(
				#version 330 core

				layout(location = 0) in vec3 a_Position;
				layout(location = 1) in vec4 a_Colour;

				uniform mat4 u_ViewProjection;

				out vec4 v_Colour;

				void main()
				{
					v_Colour = a_Colour;
					gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
				}
			)";

			std::string fragmentSrc = R"(
				#version 330 core

				in vec4 v_Colour;

				out vec4 o_Colour;

				void main()
				{
					o_Colour = v_Colour;
				}
			)";

			m_Shader.reset( Shader::Create( vertexSrc, fragmentSrc ) );
		}

		TODO( "TEMP OPENGL CALLS" );
		// Initialize Vertex Arrays and Buffers
		{
			glGenVertexArrays( 1, &m_LineVAO );
			glGenBuffers( 1, &m_LineVBO );

			glBindVertexArray( m_LineVAO );
			glBindBuffer( GL_ARRAY_BUFFER, m_LineVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( DebugLine ) * 1024, nullptr, GL_DYNAMIC_DRAW );

			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( DebugVertex ), (void*)0 );

			glEnableVertexAttribArray( 1 );
			glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( DebugVertex ), (void*)offsetof( DebugVertex, Color ) );


			glBindVertexArray( 0 );
		}

		{
			glGenVertexArrays( 1, &m_TriVAO );
			glGenBuffers( 1, &m_TriVBO );

			glBindVertexArray( m_TriVAO );
			glBindBuffer( GL_ARRAY_BUFFER, m_TriVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( DebugTri ) * 1024, nullptr, GL_DYNAMIC_DRAW );

			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( DebugVertex ), (void*)0 );

			glEnableVertexAttribArray( 1 );
			glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( DebugVertex ), (void*)offsetof( DebugVertex, Color ) );

			glBindVertexArray( 0 );
		}
	}

	void DebugDrawer::Init()
	{
		s_Instance.reset( new DebugDrawer() );
	}

	void DebugDrawer::Draw( const Matrix4& a_ViewProjection )
	{
		// Kill lines and tris that are too old
		{
			const float currentTime = Time::Now();

			for ( auto it = m_Lines.ForDuration.begin(); it != m_Lines.ForDuration.end(); )
			{
				if ( it->first < currentTime )
				{
					it = m_Lines.ForDuration.erase( it );
				}
				else
				{
					++it;
				}
			}

			for ( auto it = m_Tris.ForDuration.begin(); it != m_Tris.ForDuration.end(); )
			{
				if ( it->first < currentTime )
				{
					it = m_Tris.ForDuration.erase( it );
				}
				else
				{
					++it;
				}
			}
		}

		m_Shader->Bind();
		m_Shader->SetMatrix4( "u_ViewProjection", a_ViewProjection );

		// Draw lines
		{
			glBindVertexArray( m_LineVAO );
			glBindBuffer( GL_ARRAY_BUFFER, m_LineVBO );

			// Draw permenant lines
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( DebugLine ) * m_Lines.Permenant.size(), m_Lines.Permenant.data() );
			glDrawArrays( GL_LINES, 0, m_Lines.Permenant.size() * 2 );

			// Draw one frame lines
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( DebugLine ) * m_Lines.OneFrame.size(), m_Lines.OneFrame.data() );
			glDrawArrays( GL_LINES, 0, m_Lines.OneFrame.size() * 2 );

			// Draw for duration lines
			std::vector<DebugLine> lines;
			lines.reserve( m_Lines.ForDuration.size() );
			for ( auto&& [_, line] : m_Lines.ForDuration )
			{
				lines.push_back( line );
			}
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( DebugLine ) * lines.size(), lines.data() );
			glDrawArrays( GL_LINES, 0, lines.size() * 2 );

			glBindVertexArray( 0 );
		}

		// Draw triangles
		{
			glBindVertexArray( m_TriVAO );
			glBindBuffer( GL_ARRAY_BUFFER, m_TriVBO );

			// Draw permenant tris
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( DebugTri ) * m_Tris.Permenant.size(), m_Tris.Permenant.data() );
			glDrawArrays( GL_TRIANGLES, 0, m_Tris.Permenant.size() * 3 );

			// Draw one frame tris
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( DebugTri ) * m_Tris.OneFrame.size(), m_Tris.OneFrame.data() );
			glDrawArrays( GL_TRIANGLES, 0, m_Tris.OneFrame.size() * 3 );

			// Draw for duration tris
			std::vector<DebugTri> tris;
			tris.reserve( m_Tris.ForDuration.size() );
			for ( auto&& [_, tri] : m_Tris.ForDuration )
			{
				tris.push_back( tri );
			}
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( DebugTri ) * tris.size(), tris.data() );
			glDrawArrays( GL_TRIANGLES, 0, tris.size() * 3 );

			glBindVertexArray( 0 );
		}

		m_Lines.OneFrame.clear();
		m_Tris.OneFrame.clear();

		m_Shader->Unbind();
	}
}

#endif