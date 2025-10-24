#include "tripch.h"
#include "JoltDebugRenderer.h"
#ifdef JPH_DEBUG_RENDERER

#include "JoltUtil.h"

using namespace JPH;
using Batch = JPH::DebugRenderer::Batch;

namespace Tridium {

    JoltDebugRenderer::JoltDebugRenderer()
    {
		return;

		JPH::DebugRenderer::Initialize();

		// Create shader
		static const std::string vertexShader = R"(
			#version 330 core
			layout (location = 0) in vec4 a_Pos;
			layout (location = 1) in vec4 a_Color;
			out vec4 v_Color;
			uniform mat4 u_ViewProjection;
			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection * a_Pos;
			}
		)";

		static const std::string fragmentShader = R"(
			#version 330 core
			in vec4 v_Color;
			out vec4 o_FragColor;
			void main()
			{
				o_FragColor = v_Color;
			}
		)";

		// GEOMETRY

		std::string geometryVertexShader = R"(
			#version 330 core
			layout (location = 0) in vec3 a_Pos;
			uniform mat4 u_ViewProjection;
			void main()
			{
				gl_Position = u_ViewProjection * vec4( a_Pos, 1.0 );
			}
		)";

		std::string geometryFragmentShader = R"(
			#version 330 core
			uniform vec4 u_Color;
			out vec4 o_FragColor;
			void main()
			{
				o_FragColor = u_Color;
			}
		)";
    }

	JoltDebugRenderer::~JoltDebugRenderer()
	{
		return;
	}

    void JoltDebugRenderer::Render( const Matrix4& a_ViewProjection )
    {
		Clear();
		NextFrame();
		return;

		if ( m_Lines.empty() )
			return;

		for ( auto& call : m_GeometryCalls )
		{
			TriangleData* data = call.TriangleBatch;

			if ( data->HasIndices )
			{
			}
			else
			{
			}
		}

		Clear();
		NextFrame();
    }

	void JoltDebugRenderer::Clear()
	{
		m_Lines.clear();
		m_GeometryCalls.clear();
	}

    void JoltDebugRenderer::DrawLine( RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor )
    {
		Vector4 color( inColor.r, inColor.g, inColor.b, inColor.a );
		Line line = { 
			{ Vector4( Util::ToTridiumVec3( inFrom ), 1.0 ), color },
			{ Vector4( Util::ToTridiumVec3( inTo ), 1.0 ), color }
		};
		m_Lines.push_back( std::move( line ) );
    }

    void JoltDebugRenderer::DrawTriangle( RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor, ECastShadow inCastShadow )
    {
		DrawLine( inV1, inV2, inColor );
		DrawLine( inV2, inV3, inColor );
		DrawLine( inV3, inV1, inColor );
    }

    Batch JoltDebugRenderer::CreateTriangleBatch( const Triangle* inTriangles, int inTriangleCount )
    {
		auto* triangle_data = new TriangleData( inTriangles, inTriangleCount );
		return triangle_data;
    }

    Batch JoltDebugRenderer::CreateTriangleBatch( const Vertex* inVertices, int inVertexCount, const uint32* inIndices, int inIndexCount )
    {
		auto* triangle_data = new TriangleData( inVertices, inVertexCount, inIndices, inIndexCount );
		return triangle_data;
    }

    void JoltDebugRenderer::DrawGeometry( RMat44Arg inModelMatrix, const AABox& inWorldSpaceBounds, float inLODScaleSq, ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode )
	{
		const JPH::Array<LOD>& geometryLods = inGeometry->mLODs;
		TriangleData* triangleBatch = Cast<TriangleData*>( geometryLods[0].mTriangleBatch.GetPtr() );
		m_GeometryCalls.emplace_back( triangleBatch, Util::ToTridiumMat4( inModelMatrix ) );
    }


    void JoltDebugRenderer::DrawText3D( RVec3Arg inPosition, const string_view& inString, ColorArg inColor, float inHeight )
    {
    }

	JoltDebugRenderer::TriangleData::TriangleData( const JPH::DebugRenderer::Triangle* triangles, int num_triangles )
	{

		this->NumTriangles = num_triangles;
		this->HasIndices = false;

		for ( int i = 0; i < num_triangles; i += 1 ) {
			JPH::DebugRenderer::Triangle triangle = triangles[i];

			JPH::DebugRenderer::Vertex v1 = triangle.mV[0];
			JPH::DebugRenderer::Vertex v2 = triangle.mV[1];
			JPH::DebugRenderer::Vertex v3 = triangle.mV[2];

			TriVerts.push_back( v1.mPosition.x );
			TriVerts.push_back( v1.mPosition.y );
			TriVerts.push_back( v1.mPosition.z );

			TriVerts.push_back( v2.mPosition.x );
			TriVerts.push_back( v2.mPosition.y );
			TriVerts.push_back( v2.mPosition.z );

			TriVerts.push_back( v3.mPosition.x );
			TriVerts.push_back( v3.mPosition.y );
			TriVerts.push_back( v3.mPosition.z );
		}
	}

	JoltDebugRenderer::TriangleData::TriangleData( const JPH::DebugRenderer::Vertex* vertices, int num_vertices, const JPH::uint32* indices, int num_indices )
	{
		this->HasIndices = true;

		for ( int i = 0; i < num_vertices; i++ ) {
			JPH::DebugRenderer::Vertex vertex = vertices[i];
			this->Vertices.push_back( vertex.mPosition.x );
			this->Vertices.push_back( vertex.mPosition.y );
			this->Vertices.push_back( vertex.mPosition.z );
		}

		for ( int i = 0; i < num_indices; i++ ) {
			JPH::uint32 index = indices[i];
			//            int index = indices[i];
			this->Indices.push_back( index );
		}
	}
}

#endif