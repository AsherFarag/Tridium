#pragma once
#include <Tridium/Asset/Asset.h>
#include <Tridium/Rendering/Material.h>
#include <Tridium/Math/AABB.h>

// - Forward Declarations -
namespace Tridium { 
	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;
	class MeshCollider;

	namespace Editor { class MeshSourceImporterPanel; } 
}

namespace Tridium {

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
		SharedPtr<VertexArray> VAO;
		SharedPtr<VertexBuffer> VBO;
		SharedPtr<IndexBuffer> IBO;
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
		uint32_t MaterialIndex;
		Matrix4 Transform{ 1.0f };
		Matrix4 LocalTransform{ 1.0f }; // Do we need this?
		std::string Name;
		AABB BoundingBox;
		SharedPtr<MeshCollider> Collider;
		void GenerateMeshCollider();
	};

	void CalculateTangents( std::vector<Vertex>& a_Vertices, const std::vector<uint32_t>& a_Indices );

	// - Mesh Source -
	// A mesh source is a collection of assets such as meshes, materials, textures, animations and skeletons,
	// loaded from a file format such as FBX or glTF.
	class MeshSource final : public Asset
	{
	public:
		ASSET_CLASS_TYPE( MeshSource );
		MeshSource() = default;
		MeshSource( const std::vector<Vertex>& a_Vertices, const std::vector<uint32_t>& a_Indices, const Matrix4& a_Transform );
		virtual ~MeshSource() = default;

		const SubMesh& GetSubMesh( uint32_t a_Index ) const { return m_SubMeshes[a_Index]; }
		std::vector<SubMesh>& GetSubMeshes() { return m_SubMeshes; }
		const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }
		const std::vector<MaterialHandle>& GetMaterials() const { return m_Materials; }
		const AABB& GetBoundingBox() const { return m_BoundingBox; }

	private:
		std::vector<SubMesh> m_SubMeshes;
		std::vector<MaterialHandle> m_Materials;
		AABB m_BoundingBox;

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
		StaticMesh( MeshSourceHandle a_MeshSource );
		StaticMesh( MeshSourceHandle a_MeshSource, const std::vector<uint32_t>& a_SubMeshes );
		virtual ~StaticMesh() = default;

		void SetMeshSource( MeshSourceHandle a_MeshSource ) { m_MeshSource = a_MeshSource; }
		MeshSourceHandle GetMeshSource() const { return m_MeshSource; }
		void SetSubMeshes( const std::vector<uint32_t>& a_SubMeshes );
		void SetSubMeshes( SharedPtr<MeshSource> a_MeshSource );
		const auto& GetSubMeshes() const { return m_SubMeshes; }
		void SetMaterials( const std::vector<MaterialHandle>& a_Materials ) { m_Materials = a_Materials; }
		auto& GetMaterials() { return m_Materials; }
		const auto& GetMaterials() const { return m_Materials; }
		void SetBoundingBox( const AABB& a_BoundingBox ) { m_BoundingBox = a_BoundingBox; }
		const AABB& GetBoundingBox() const { return m_BoundingBox; }

	private:
		MeshSourceHandle m_MeshSource;
		std::vector<uint32_t> m_SubMeshes;
		std::vector<MaterialHandle> m_Materials;
		AABB m_BoundingBox;
	};

	// - Mesh Factory -
	// A factory class for creating common mesh types such as quads and cubes.
	class MeshFactory
	{
	public:
		static void Init();

		static SharedPtr<MeshSource> CreateQuad( const Vector2& a_Size = Vector2( 1.0f ) );
		static SharedPtr<MeshSource> CreateCube( const Vector3& a_Size = Vector3( 1.0f ) );
		static SharedPtr<MeshSource> CreateSphere( float a_Radius = 0.5f, uint32_t a_Stacks = 16, uint32_t a_Slices = 16 );
		static SharedPtr<MeshSource> CreateCylinder( float a_BaseRadius = 0.5f, float a_TopRadius = 0.5f, float a_Height = 1.0f, uint32_t a_Stacks = 16, uint32_t a_Slices = 16 );
		static SharedPtr<MeshSource> CreateCapsule( float a_Radius = 0.5f, float a_Height = 1.0f, uint32_t a_Stacks = 16, uint32_t a_Slices = 16, uint32_t a_SphereStacks = 16, uint32_t a_SphereSlices = 16 );
		static SharedPtr<MeshSource> CreateCone( float a_Radius = 0.5f, float a_Height = 1.0f, uint32_t a_Stacks = 16, uint32_t a_Slices = 16 ) { return CreateCylinder( a_Radius, 0.0f, a_Height, a_Stacks, a_Slices ); }
		static SharedPtr<MeshSource> CreateTorus( float a_Radius = 0.5f, float a_Radius2 = 0.25f, uint32_t a_Stacks = 32, uint32_t a_Slices = 32 );

		static AssetHandle GetDefaultQuad();
		static AssetHandle GetDefaultCube();
		static AssetHandle GetDefaultSphere();
		static AssetHandle GetDefaultCylinder();
		static AssetHandle GetDefaultCapsule();
		static AssetHandle GetDefaultCone();
		static AssetHandle GetDefaultTorus();
	};
}