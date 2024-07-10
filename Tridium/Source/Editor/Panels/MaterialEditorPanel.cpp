#include "tripch.h"
#ifdef IS_EDITOR
#include "Editor/EditorUtil.h"
#include "MaterialEditorPanel.h"
#include <Tridium/IO/MaterialSerializer.h>

namespace Tridium::Editor {
	MaterialEditorPanel::MaterialEditorPanel( const MaterialHandle& material )
		: Panel("Material Editor##")
	{
		m_Material = material;
	}

	// Returns true if modified
	bool DrawTextureDragDrop(const char* label, TextureHandle& handle)
	{
		auto oldHandle = handle;

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

		return oldHandle != handle;
	}

	void MaterialEditorPanel::OnImGuiDraw()
	{
		auto material = MaterialLibrary::GetMaterial( m_Material );
		ImGuiWindowFlags flags = ( material && material->IsModified() ) ? ImGuiWindowFlags_UnsavedDocument : 0;
		if ( ImGuiBegin( flags ) && material )
		{
			bool modified = material->IsModified();
			modified |= ImGui::ColorEdit4( "Color", &material->Color[0] );
			modified |= ImGui::SliderFloat( "Reflectivity", &material->Reflectivity, 0.0f, 1.0f );
			modified |= ImGui::SliderFloat( "Refraction", &material->Refraction, 0.0f, 1.0f );

			ImGui::Separator();

			modified |= DrawTextureDragDrop( "Base Color: ", material->BaseColorTexture );
			modified |= DrawTextureDragDrop( "Normal Map: ", material->NormalMapTexture );
			modified |= DrawTextureDragDrop( "Metallic: ", material->MetallicTexture );
			modified |= DrawTextureDragDrop( "Roughness: ", material->RoughnessTexture );
			modified |= DrawTextureDragDrop( "Emissive: ", material->EmissiveTexture );

			material->SetModified( modified );
		}

		m_IsHovered = ImGui::IsWindowHovered();
		m_IsFocused = ImGui::IsWindowFocused();

		ImGuiEnd();
	}

	bool MaterialEditorPanel::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		bool control = Input::IsKeyPressed( Input::KEY_LEFT_CONTROL );

		switch ( e.GetKeyCode() )
		{
		case Input::KEY_S:
		{
			if ( control )
			{
				if ( auto mat = MaterialLibrary::GetMaterial(m_Material) )
				{
					MaterialSerializer s( mat );
					s.SerializeText( mat->GetPath() );
					return true;
				}
			}
		}
		}

		return false;
	}

}

#endif // IS_EDITOR