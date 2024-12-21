#pragma once
#ifdef JPH_DEBUG_RENDERER

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>

namespace Tridium {

	// Forward declarations
	class Shader;
	class VertexArray;
	// --------------------

	class JoltDebugRenderer final : public JPH::DebugRenderer
	{
	public:
		JoltDebugRenderer();
		virtual ~JoltDebugRenderer();

		void Render( const Matrix4& a_ViewProjection );
		void Clear();

		virtual void DrawLine( JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor ) override;
		virtual void DrawTriangle( JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow ) override;
		virtual Batch CreateTriangleBatch( const Triangle* inTriangles, int inTriangleCount ) override;
		virtual Batch CreateTriangleBatch( const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount ) override;
		virtual void DrawGeometry( JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode ) override;
		virtual void DrawText3D( JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight ) override;

	private:
		struct GizmoVertex
		{
			Vector4 Position;
			Vector4 Color;
		};

		struct Line
		{
			GizmoVertex From;
			GizmoVertex To;
		};

		std::vector<Line> m_Lines;
		TODO( "Remove OpenGL specific code from JoltDebugRenderer" );
		uint32_t m_LineVAO;
		uint32_t m_LineVBO;

		class TriangleData : public JPH::RefTargetVirtual, public JPH::RefTarget<TriangleData>
		{
		public:
			int NumTriangles;
			std::vector<float> TriVerts;
			bool HasIndices;
			std::vector<float> Vertices;
			std::vector<JPH::uint32> Indices;

			TriangleData( const JPH::DebugRenderer::Triangle* triangles, int num_triangles );
			TriangleData( const JPH::DebugRenderer::Vertex* vertices, int num_vertices, const JPH::uint32* indices, int num_indices );

			virtual void AddRef() override { ++mRefCount; }
			virtual void Release() override {
				if ( --mRefCount == 0 )
					delete this;
			}
		};

		struct GeometryDrawCall
		{
			TriangleData* TriangleBatch;
			Matrix4 ModelMatrix;
		};

		std::vector<GeometryDrawCall> m_GeometryCalls;

		struct
		{
			uint32_t VAO;
			uint32_t VBO;
			uint32_t IBO;
		} m_Geometry;


		SharedPtr<Shader> m_Shader;
		SharedPtr<Shader> m_GeometryShader;
	};

}

#endif