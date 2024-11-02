#pragma once
#include <Tridium/IO/FilePath.h>
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Core/Memory.h>
#include <Tridium/Asset/ImportSettings.h>

namespace Tridium {

	class AssimpImporter final
	{
	public:
		AssimpImporter(const IO::FilePath& a_FilePath);

		SharedPtr<MeshSource> ImportMeshSource( const MeshSourceImportSettings& a_ImportSettings = {} );

	private:
		void ProcessNode( SharedPtr<MeshSource>& a_MeshSource, const void* a_AssimpScene, void* a_AssimpNode, const Matrix4& a_ParentTransform );
		SubMesh ProcessSubMesh( SharedPtr<MeshSource>& a_MeshSource, const void* a_AssimpScene, void* a_AssimpMesh );

		//MeshSource::MeshNode* ProcessNode( void* a_AssimpScene, void* a_AssimpNode, SharedPtr<MeshSource>& a_MeshSource, uint32_t a_ParentNodeIndex, const Matrix4& a_ParentTransform = Matrix4( 1.0f ) );
		//SubMesh ProcessSubMesh( void* a_AssimpScene, void* a_AssimpMesh, SharedPtr<MeshSource>& a_MeshSource );

		//void ExtractSubmeshes( void* a_AssimpScene, SharedPtr<MeshSource>& a_MeshSource);
		//void ExtractMaterials( void* a_AssimpScene, SharedPtr<MeshSource>& a_MeshSource );
		//AssetHandle ExtractTexture( void* a_AssimpScene, void* a_AssimpMaterial, int a_AssimpTextureType );

		//void TraverseNodes( SharedPtr<MeshSource> a_MeshSource, void* a_AssimpNode, uint32_t a_NodeIndex, const Matrix4& a_ParentTransform = Matrix4( 1.0f ), uint32_t a_Level = 0 );

	private:
		const IO::FilePath m_FilePath;
	};

}