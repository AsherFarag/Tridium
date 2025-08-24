#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Core/Core.h>

namespace Tridium::T {

	// Forward declarations
	class Material;

	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector3 Tangent;
		Vector2 TexCoord;
	};

	//====================================
	// SubMesh
	//  Represents a single drawable part of a mesh source asset.
	struct SubMesh
	{
		size_t BaseVertexIndex = 0;     
		size_t VertexCount = 0;         
		size_t BaseIndex = 0;           
		size_t IndexCount = 0;          
		uint32_t MaterialIndex = 0;     
		Matrix4 WorldTransform{ 1.0f }; // Relative to the root of the mesh
		AABB BoundingBox{};             
		String Name{};                  

		void CalculateBoundingBox( Span<const Vertex> a_Vertices, Span<const uint32_t> a_Indices )
		{
			ASSERT( BaseIndex + IndexCount <= a_Indices.size(), "SubMesh index indices out of bounds." );
			ASSERT( BaseVertexIndex + VertexCount <= a_Vertices.size(), "SubMesh vertex indices out of bounds." );
			BoundingBox = {};

			if ( IndexCount > 0 ) // If this submesh is indexed, use the indexed vertices
			{
				for ( size_t i = BaseIndex; i < BaseIndex + IndexCount; ++i )
				{
					BoundingBox.Expand( a_Vertices[a_Indices[i]].Position );
				}
			}
			else // If this submesh is not indexed, use the vertex array directly
			{
				for ( size_t i = BaseVertexIndex; i < BaseVertexIndex + VertexCount; ++i )
				{
					BoundingBox.Expand( a_Vertices[i].Position );
				}
			}
		}
	};

	class MeshSource : public IAsset
	{
	public:
		static SharedPtr<MeshSource> Create() { return MakeShared<EnableMakeShared<MeshSource>>(); }
		static SharedPtr<MeshSource> Create( Span<Vertex> a_Vertices, Span<uint32_t> a_Indices, const Matrix4& a_ModelTransform = Matrix4( 1.0f ) );
		static constexpr EAssetType StaticType() { return EAssetType::MeshSource; }
		EAssetType Type() const override { return StaticType(); }
		bool Valid() const override { return !m_SubMeshes.Empty(); }

		const auto& Vertices() const { return m_Vertices; }
		const auto& Indices() const { return m_Indices; }
		const auto& SubMeshes() const { return m_SubMeshes; }
		const auto& Materials() const { return m_Materials; }
		const auto& BoundingBox() const { return m_BoundingBox; }

	private:
		Array<Vertex> m_Vertices{};
		Array<uint32_t> m_Indices{};
		Array<SubMesh> m_SubMeshes{};
		Array<SharedPtr<Material>> m_Materials{};
		AABB m_BoundingBox{};

		void UpdateBoundingBox();
	};

	class StaticMesh : public IAsset
	{
	public:
		struct MeshChunk
		{
			uint32_t SubMeshIndex = 0; // Index of the submesh in the source mesh
			SharedPtr<Material> OverrideMaterial; // Optional override material for this submesh
		};

		static SharedPtr<StaticMesh> Create() { return MakeShared<EnableMakeShared<StaticMesh>>(); }
		static SharedPtr<StaticMesh> Create( SharedPtr<MeshSource> a_SourceMesh, Span<const MeshChunk> a_MeshChunks );
		static constexpr EAssetType StaticType() { return EAssetType::StaticMesh; }
		EAssetType Type() const override { return StaticType(); }
		bool Valid() const override { return m_SourceMesh != nullptr && !m_MeshChunks.Empty(); }

		const auto& SourceMesh() const { return m_SourceMesh; }
		const auto& SubMeshes() const { return m_MeshChunks; }
		AABB BoundingBox() const { return m_BoundingBox; }
		void Update( SharedPtr<MeshSource> a_SourceMesh, Span<const MeshChunk> a_MeshChunks );

	private:
		SharedPtr<MeshSource> m_SourceMesh;
		Array<MeshChunk> m_MeshChunks;
		AABB m_BoundingBox; 

		void UpdateBoundingBox();
	};

} // namespace Tridium