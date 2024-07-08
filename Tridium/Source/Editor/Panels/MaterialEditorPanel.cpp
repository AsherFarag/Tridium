#include "tripch.h"
#ifdef IS_EDITOR
#include "Editor/EditorUtil.h"
#include "MaterialEditorPanel.h"

namespace Tridium::Editor {
	MaterialEditorPanel::MaterialEditorPanel( const MaterialHandle& material )
		: Panel("Material Editor##")
	{
		m_Material = material;
	}

	void DrawTextureDragDrop(const char* label, TextureHandle& handle)
	{
		Ref<Texture> sprite = TextureLibrary::GetTexture( handle );
		bool hasSprite = sprite != nullptr;
		ImGui::DragDropSelectable( label, hasSprite, hasSprite ? sprite->GetPath().c_str() : "Null", TE_PAYLOAD_CONTENT_BROWSER_ITEM,
			[&]( const ImGuiPayload* payload )
			{
				std::string filePath = static_cast<const char*>( payload->Data );
				handle = Editor::Util::GetTextureHandle( filePath );
			} );

		// On right click, give the option to remove the sprite, if there is one.
		if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
			ImGui::OpenPopup( "##RemoveSprite" );
		if ( ImGui::BeginPopup( "##RemoveSprite" ) )
		{
			if ( ImGui::MenuItem( "Remove Sprite", nullptr, nullptr, hasSprite ) )
				handle = {};

			ImGui::EndPopup();
		}
	}

	void MaterialEditorPanel::OnImGuiDraw()
	{
		auto material = MaterialLibrary::GetMaterial( m_Material );
		if ( ImGuiBegin() && material )
		{
			ImGui::ColorEdit3( "Ambient", &material->Ambient[0] );
			ImGui::ColorEdit3( "Diffuse", &material->Diffuse[0] );
			ImGui::ColorEdit3( "Specular", &material->Specular[0] );
			ImGui::SliderFloat( "Opacity", &material->Opacity, 0.0f, 1.0f );
			ImGui::SliderFloat( "Reflectivity", &material->Reflectivity, 0.0f, 1.0f );
			ImGui::SliderFloat( "Refraction", &material->Refraction, 0.0f, 1.0f );

			ImGui::Separator();

			DrawTextureDragDrop( "Base Color: ", material->BaseColorTexture );
			DrawTextureDragDrop( "Normal Map: ", material->NormalMapTexture );
			DrawTextureDragDrop( "Metallic: ", material->MetallicTexture );
			DrawTextureDragDrop( "Roughness: ", material->RoughnessTexture );
			DrawTextureDragDrop( "Emissive: ", material->EmissiveTexture );
		}
		else
		{
			ImGui::Text( "No Material!" );
		}

		ImGuiEnd();
	}

}

#endif // IS_EDITOR