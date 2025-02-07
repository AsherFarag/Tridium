#pragma once
#include <Tridium/IO/FilePath.h>
#include <Tridium/Graphics/Rendering/Mesh.h>
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
		void ExtractMaterials( void* a_AssimpScene, SharedPtr<MeshSource>& a_MeshSource );
		TextureHandle ExtractTexture( void* a_AssimpScene, void* a_AssimpMaterial, int a_AssimpTextureType );

	private:
		const IO::FilePath m_FilePath;
	};

}