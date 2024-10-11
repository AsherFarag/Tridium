#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Rendering/Material.h>

namespace Tridium {

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;

	struct Vertex
	{
		static constexpr Byte NumUVChannels = 8;

		Vector3 Position;
		Vector3 Normal;
		Vector2 UV[NumUVChannels];
		Vector3 Tangent;
	};

	class Mesh : public Asset
	{
	public:
		ASSET_CLASS_TYPE( Mesh )
		ASSET_LOADER_TYPE( ModelLoader )

		static const AssetRef<Mesh>& GetQuad();

		inline auto GetVAO() const { return m_VAO; }
		inline auto GetVBO() const { return m_VBO; }
		inline auto GetIBO() const { return m_IBO; }

		const inline size_t GetNumOfVerticies() const { return m_NumVerticies; }
		const inline size_t GetNumOfPolygons() const { return (size_t)( m_NumVerticies / 3u ); }

	//private:
		SharedPtr<VertexArray> m_VAO;
		SharedPtr<VertexBuffer> m_VBO;
		SharedPtr<IndexBuffer> m_IBO;

		size_t m_NumVerticies = 0;
		size_t m_NumIndicies = 0;

		std::vector<Vertex> m_Verticies;

		struct SubMesh
		{
			SharedPtr<IndexBuffer> IBO;
			uint32_t MaterialIndex;
		};
		std::vector<SubMesh> m_SubMeshes;
		std::vector<AssetRef<Material>> m_Materials;
	};
}