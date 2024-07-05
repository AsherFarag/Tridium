#pragma once
#include "Tridium/Core/Core.h"

namespace Tridium {

	typedef GUID MeshHandle;

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;

	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector2 UV;
		Vector3 Tangent;
	};

	struct MeshImportSettings
	{
		MeshImportSettings();
		unsigned int PostProcessFlags;
		float Scale = 1.f;
		bool DiscardLocalData = false; /* If true, once the mesh has been loaded onto the GPU, the local Vertex Data will be deleted. */
	};

	struct SubMesh
	{
		Ref<IndexBuffer> IBO;
		uint32_t MaterialIndex;
	};

	class Mesh
	{
		friend class MeshLoader;
		friend class MeshLibrary;

	public:
		inline auto GetVAO() const { return m_VAO; }
		inline auto GetVBO() const { return m_VBO; }
		inline auto GetIBO() const { return m_IBO; }

		const inline auto& GetPath() const { return m_FilePath; }

		const inline size_t GetNumOfVerticies() const { return m_NumVerticies; }
		const inline size_t GetNumOfPolygons() const { return (size_t)( m_NumVerticies / 3u ); }

		const inline MeshImportSettings& GetImportSettings() const { return m_ImportSettings; }

	private:
		std::string m_FilePath;
		MeshImportSettings m_ImportSettings;

		Ref<VertexArray> m_VAO;
		Ref<VertexBuffer> m_VBO;
		Ref<IndexBuffer> m_IBO;

		size_t m_NumVerticies = 0;

		std::vector<Vertex> m_Verticies;
		std::vector<SubMesh> m_SubMeshes;
		// std::vector<Material> m_Materials;
	};

	class MeshLoader
	{
	public:
		static bool Import( const std::string& filepath, MeshHandle& outMeshHandle );
		static bool Import( const std::string& filepath, MeshHandle& outMeshHandle, const MeshImportSettings& importSettings );
	};

	class MeshLibrary
	{
		friend MeshLoader;

	public:
		static MeshLibrary* Get();
		static Ref<Mesh> GetMesh( const MeshHandle& meshHandle );
		static bool GetHandle( const std::string& filePath, MeshHandle& outMeshHandle );

		// - Primatives -
		static inline MeshHandle GetQuad() { return Get()->m_Quad; }

	private:
		void InitPrimatives();

		static void AddMesh( const std::string& filePath, const Ref<Mesh>& mesh, MeshHandle& outMeshHandle );

	private:
		std::unordered_map<std::string, MeshHandle> m_PathHandles;
		std::unordered_map<MeshHandle, Ref<Mesh>> m_LoadedMeshes;

		// - Primatives -
		MeshHandle m_Quad;
	};
}