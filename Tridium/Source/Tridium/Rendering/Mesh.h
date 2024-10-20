#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Rendering/Material.h>

// - Forward Declarations -
namespace Tridium { namespace Editor { class MeshSourceImporterPanel; } }

namespace Tridium {

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;

	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector3 Bitangent;
		Vector3 Tangent;
		Vector2 UV;
	};

	struct SubMesh
	{
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t NumVerticies = 0;
		uint32_t NumIndicies = 0;
		Matrix4 Transform;
		Matrix4 LocalTransform;
		std::string Name;
	};

	// - Mesh Source -
	// A mesh source is a collection of assets such as meshes, materials, textures, animations and skeletons,
	// loaded from a file format such as FBX or glTF.
	class MeshSource final : public Asset
	{
	public:
		ASSET_CLASS_TYPE( MeshSource );
		MeshSource() = default;
		MeshSource( const std::vector<Vertex>& a_Vertices, const std::vector<uint32_t>& a_Indices, const Matrix4& a_Transform );
		MeshSource( const std::vector<Vertex>& a_Vertices, const std::vector<uint32_t>& a_Indices, const std::vector<SubMesh>& a_SubMeshes );
		virtual ~MeshSource() = default;

		SharedPtr<VertexArray>  GetVAO() const { return m_VAO; }
		SharedPtr<VertexBuffer> GetVBO() const { return m_VBO; }
		SharedPtr<IndexBuffer>  GetIBO() const { return m_IBO; }

		const auto& GetVertices() const { return m_Vertices; }
		const auto& GetIndices() const { return m_Indices; }

		std::vector<SubMesh>& GetSubMeshes() { return m_SubMeshes; }
		const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }

		const std::vector<AssetHandle>& GetMaterials() const { return m_Materials; }

	private:
		// - GPU Handles -

		SharedPtr<VertexArray> m_VAO;
		SharedPtr<VertexBuffer> m_VBO;
		SharedPtr<IndexBuffer> m_IBO;

		// - CPU Data -

		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		std::vector<SubMesh> m_SubMeshes;
		std::vector<AssetHandle> m_Materials;

		// A mesh node is a node in a hierarchy of nodes that make up a mesh, loaded by Assimp.
		struct MeshNode
		{
			Matrix4 LocalTransform;
			uint32_t Parent = MAXUINT32;
			std::vector<uint32_t> SubMeshes;
			std::vector<uint32_t> Children;
			std::string Name;

			bool IsRoot() const { return Parent == MAXUINT32; }
		};

		std::vector<MeshNode> m_MeshNodes;

		friend class AssimpImporter;
		friend class Editor::MeshSourceImporterPanel;
	};

	// - Static Mesh -
	// A static mesh is an unanimated mesh that is not deformed by bones or other deformations.
	// It can still be composed of multiple sub-meshes and materials.
	class StaticMesh final : public Asset
	{
	public:
		ASSET_CLASS_TYPE( StaticMesh );
		StaticMesh( AssetHandle a_MeshSource );
		StaticMesh( AssetHandle a_MeshSource, const std::vector<uint32_t>& a_SubMeshes );
		virtual ~StaticMesh() = default;

		void SetMeshSource( AssetHandle a_MeshSource ) { m_MeshSource = a_MeshSource; }
		AssetHandle GetMeshSource() const { return m_MeshSource; }
		void SetSubMeshes( const std::vector<uint32_t>& a_SubMeshes );
		void SetSubMeshes( SharedPtr<MeshSource> a_MeshSource );
		const auto& GetSubMeshes() const { return m_SubMeshes; }
		void SetMaterials( const std::vector<AssetHandle>& a_Materials ) { m_Materials = a_Materials; }
		auto& GetMaterials() { return m_Materials; }

	private:
		SharedPtr<VertexArray> m_VAO;
		SharedPtr<IndexBuffer> m_IBO;

		AssetHandle m_MeshSource;
		std::vector<uint32_t> m_SubMeshes;
		std::vector<AssetHandle> m_Materials;
	};

	// - Mesh Factory -
	// A factory class for creating common mesh types such as quads and cubes.
	class MeshFactory
	{
	public:
		static SharedPtr<VertexArray> CreateQuad();
		static SharedPtr<VertexArray> CreateCube();

		static AssetHandle GetQuad();
		static AssetHandle GetCube();
	};
}