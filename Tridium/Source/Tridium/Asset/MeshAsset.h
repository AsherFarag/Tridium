#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Core/Core.h>

namespace Tridium {

	TODO( "I don't like the idea of static meshes being forced to use this vertex format."
		  "Eventually, meshes should just store a void* to their vertex data and a vertex layout describing it."
		  "This also requires the model importer to be able to convert to arbitrary vertex formats." );

	static constexpr uint32_t InvalidMaterialIndex = ( uint32_t )-1;

	//=================================================================================================
	// Vertex: The default vertex format used by MeshSource assets.
	//=================================================================================================
	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector3 Tangent;
		Vector2 TexCoord;
	};

	//=================================================================================================
	// SubMesh: Represents a single drawable part of a mesh source asset.
	//=================================================================================================
	struct SubMesh
	{
		size_t BaseVertexIndex = 0;     
		size_t VertexCount = 0;         
		size_t BaseIndex = 0;           
		size_t IndexCount = 0;          
		uint32_t MaterialIndex = 0;     
		Matrix4 Transform{ 1.0f };
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

	//=================================================================================================
	// Static Mesh Asset
	//=================================================================================================
	DEFINE_ASSET_TYPE( StaticMesh )
	{
	public:

		struct LOD
		{
			Array<Vertex> Vertices{};
			Array<uint32_t> Indices{};
		};

		bool Valid() const override { return !m_SubMeshes.Empty() && !m_Materials.Empty(); }

		const auto& LODs() const { return m_LODs; }
		const auto& SubMeshes() const { return m_SubMeshes; }
		const auto& Materials() const { return m_Materials; }
		const auto& BoundingBox() const { return m_BoundingBox; }

		AssetRef<class Material> GetMaterial( uint32_t a_Index ) const
		{
			if ( a_Index >= m_Materials.Size() )
			{
				return nullptr;
			}

			return m_Materials[ a_Index ];
		}

		AssetRef<class Material> GetMaterial( const SubMesh& a_SubMesh ) const
		{
			return GetMaterial( a_SubMesh.MaterialIndex );
		}

	protected:

		//=============================================================================================
		TODO( "LODs will need their own submeshes eventually." );
		Array<LOD> m_LODs{};
		Array<SubMesh> m_SubMeshes{};
		Array<AssetRef<class Material>> m_Materials{};
		AABB m_BoundingBox{};

		void UpdateBoundingBox();
	};

} // namespace Tridium