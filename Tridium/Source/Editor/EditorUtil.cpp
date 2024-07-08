#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorUtil.h"
#include "EditorLayer.h"

#include "Panels/MeshImporterPanel.h"


namespace Tridium::Editor::Util
{
	MeshHandle GetMeshHandle( const std::string& filePath )
	{
		MeshHandle handle;

		if ( MeshLibrary::GetMeshHandle( filePath, handle ) )
			return handle;

		// The mesh has not been imported, so create a Mesh Import Prompt
		handle = MeshHandle::Create();
		EditorLayer::PushPanel<MeshImporterPanel>( filePath, handle );

		return handle;
	}

	TextureHandle GetTextureHandle( const std::string& filePath )
	{
		TextureHandle handle;
		if ( TextureLibrary::GetTextureHandle( filePath, handle ) )
			return handle;

		if ( auto tex = TextureLoader::Import( filePath ) )
		{
			handle = TextureHandle::Create();
			tex->_SetPath( filePath );
			tex->_SetHandle( handle );
			TextureLibrary::AddTexture( filePath, tex );
		}

		return handle;
	}

	MaterialHandle GetMaterialHandle( const std::string& filePath )
	{
		MaterialHandle handle;
		if ( MaterialLibrary::GetMaterialHandle( filePath, handle ) )
			return handle;

		if ( auto mat = MaterialLoader::Import( filePath ) )
		{
			handle = mat->GetHandle();
			mat->_SetPath( filePath );
			MaterialLibrary::AddMaterial( filePath, mat );
		}

		return handle;
	}
};

#endif // IS_EDITOR