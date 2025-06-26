#include "tripch.h"
#include "MeshAsset.h"
#include "AssetDatabase.h"

namespace Tridium::T {

	//// MeshSource ////

	SharedPtr<MeshSource> MeshSource::Create( Span<Vertex> a_Vertices, Span<uint32_t> a_Indices, const Matrix4& a_ModelTransform )
	{
		SharedPtr<MeshSource> asset = Create();
		asset->m_Vertices = a_Vertices;
		asset->m_Indices = a_Indices;

		SubMesh& submesh = asset->m_SubMeshes.EmplaceBack();
		submesh.WorldTransform = a_ModelTransform;
		submesh.VertexCount = a_Vertices.size();
		submesh.IndexCount = a_Indices.size();
		submesh.CalculateBoundingBox( a_Vertices, a_Indices );

		asset->UpdateBoundingBox();

		return asset;
	}

	void MeshSource::UpdateBoundingBox()
	{
		m_BoundingBox = AABB{};

		for (const auto& subMesh : m_SubMeshes)
		{
			m_BoundingBox.Expand( subMesh.BoundingBox );
		}
	}

	//// StaticMesh ////

	SharedPtr<StaticMesh> StaticMesh::Create( SharedPtr<MeshSource> a_SourceMesh, Span<const MeshChunk> a_MeshChunks )
	{
		auto asset = Create();
		asset->Update( a_SourceMesh, a_MeshChunks );
		return asset;
	}

	void StaticMesh::Update( SharedPtr<MeshSource> a_SourceMesh, Span<const MeshChunk> a_MeshChunks )
	{
		m_SourceMesh = std::move( a_SourceMesh );
		m_MeshChunks.Clear();

		if ( m_SourceMesh )
		{
			m_MeshChunks.Reserve( a_MeshChunks.size() );
			for ( const MeshChunk& chunk : a_MeshChunks )
			{
				if ( chunk.SubMeshIndex < m_SourceMesh->SubMeshes().Size() )
				{
					m_MeshChunks.EmplaceBack( chunk.SubMeshIndex, chunk.OverrideMaterial );
				}
				else
				{
					LOG( LogCategory::Asset, Warn, "Invalid submesh index {} in mesh asset '{}'",
						chunk.SubMeshIndex, AssetDatabase::GetAssetName( m_SourceMesh->ID() ) );
				}
			}
		}

		UpdateBoundingBox();
	}

	void StaticMesh::UpdateBoundingBox()
	{
		m_BoundingBox = AABB{};

		if ( !m_SourceMesh )
			return;

		for ( const auto& [submeshIndex, overrideMaterial] : m_MeshChunks )
		{
			const SubMesh& subMesh = m_SourceMesh->SubMeshes().At( submeshIndex );
			m_BoundingBox.Expand( subMesh.BoundingBox );
		}
	}

} // namespace Tridium