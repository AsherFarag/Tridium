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

		if ( MeshLibrary::GetHandle( filePath, handle ) )
			return handle;

		// The mesh has not been imported, so create a Mesh Import Prompt
		handle = GUID::Create();
		EditorLayer::PushPanel<MeshImporterPanel>( filePath, handle );

		return handle;
	}
};

#endif // IS_EDITOR