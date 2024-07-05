#include "tripch.h"
#ifdef IS_EDITOR
#include "EditorUtil.h"
#include "EditorLayer.h"

#include "Panels/MeshImporterPanel.h"


namespace Tridium::Editor::Util
{
	MeshHandle GetMesh( const std::string& filePath )
	{
		MeshHandle handle;

		if ( MeshLibrary::GetHandle( filePath, handle ) )
			return handle;
		else
			EditorLayer::PushPanel<MeshImporterPanel>( filePath );

		return handle;
	}
};

#endif // IS_EDITOR