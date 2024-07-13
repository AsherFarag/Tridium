#pragma once
#include <Tridium/Core/Asset.h>
#include "Material.h"

namespace Tridium {

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
	public:
		ASSET_CLASS_TYPE( Mesh )
		static bool Load( const std::string& path );

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
	};
}