#include "tripch.h"
#ifdef IS_EDITOR

#include "MaterialEditorPanel.h"

namespace Tridium::Editor {
	MaterialEditorPanel::MaterialEditorPanel( const MaterialHandle& material )
		: Panel("Material Editor##")
	{
		m_Material = material;
	}

	void MaterialEditorPanel::OnImGuiDraw()
	{
		if ( ImGuiBegin() )
		{
			auto material = MaterialLibrary::GetMaterial( m_Material );
			if ( material )
			{
				ImGui::ColorEdit3( "Ambient", &material->Ambient[0] );
			}
		}

		ImGuiEnd();
	}

}

#endif // IS_EDITOR