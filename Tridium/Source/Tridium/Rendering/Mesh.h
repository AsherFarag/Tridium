#pragma once
#include <Tridium/Core/Asset.h>
#include "Material.h"

namespace Tridium {

	using MeshHandle = AssetHandle;

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;

	struct Vertex
	{
		static constexpr size_t NumUVChannels = 8;

		Vector3 Position;
		Vector3 Normal;
		Vector2 UV[NumUVChannels];
		Vector3 Tangent;
	};

	class Mesh : public Asset
	{
		friend class MeshLoader;
		friend class MeshLibrary;

	public:
		inline auto GetVAO() const { return m_VAO; }
		inline auto GetVBO() const { return m_VBO; }
		inline auto GetIBO() const { return m_IBO; }

		const inline size_t GetNumOfVerticies() const { return m_NumVerticies; }
		const inline size_t GetNumOfPolygons() const { return (size_t)( m_NumVerticies / 3u ); }

	private:
		Ref<VertexArray> m_VAO;
		Ref<VertexBuffer> m_VBO;
		Ref<IndexBuffer> m_IBO;

		size_t m_NumVerticies = 0;
		size_t m_NumIndicies = 0;

		std::vector<Vertex> m_Verticies;

		struct SubMesh
		{
			Ref<IndexBuffer> IBO;
			uint32_t MaterialIndex;
		};
		std::vector<SubMesh> m_SubMeshes;
		std::vector<MaterialHandle> m_Materials;
	};


	struct MeshImportSettings
	{
		MeshImportSettings();
		unsigned int PostProcessFlags;
		float Scale = 1.f;
		bool DiscardLocalData = false; /* If true, once the mesh has been loaded onto the GPU, the local Vertex Data will be deleted. */
	};

	class MeshLoader
	{
	public:
		static Ref<Mesh> Import( const std::string& filepath, const MeshImportSettings& importSettings = {} );

		static MeshHandle Load( const std::string& filepath, const MeshImportSettings& importSettings = {} );
	};

	class MeshLibrary : public AssetLibrary<MeshLibrary, MeshHandle, Mesh>
	{
		friend MeshLoader;
	public:
		static inline Ref<Mesh> GetMesh( const MeshHandle& meshHandle ) { return Get().GetAsset( meshHandle ); }
		static inline bool GetMeshHandle( const std::string& path, MeshHandle& outMeshHandle ) { return Get().GetHandle( path, outMeshHandle ); }
		static inline MeshHandle GetMeshHandle( const std::string& path ) { return Get().GetHandle( path ); }
		static inline bool HasMeshHandle( const std::string& path ) { return Get().HasHandle( path ); }
		static inline bool AddMesh( const std::string& path, const Ref<Mesh>& mesh ) { return Get().AddAsset( path, mesh ); }
		static inline bool RemoveMesh( const MeshHandle& meshHandle ) { return Get().RemoveAsset( meshHandle ); }

		// - Primatives -
		static inline MeshHandle GetQuad() { return Get().m_Quad; }
		static inline MeshHandle GetCube() { return Get().m_Cube; }
		static inline MeshHandle GetSphere() { return Get().m_Sphere; }

		virtual void Init() override;

	private:
		void InitPrimatives();

		// - Primatives -
		MeshHandle m_Quad;
		MeshHandle m_Cube;
		MeshHandle m_Sphere;
	};
}