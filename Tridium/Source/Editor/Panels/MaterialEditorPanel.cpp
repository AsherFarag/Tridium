#include "tripch.h"
#ifdef IS_EDITOR
#include "Editor/EditorUtil.h"
#include "MaterialEditorPanel.h"
#include <Tridium/IO/MaterialSerializer.h>

namespace Tridium::Editor {
	MaterialEditorPanel::MaterialEditorPanel( const SharedPtr<Material>& material )
		: Panel( "Material Editor##" ), m_Material( material )
	{
	}

	// Returns true if modified
	bool DrawTextureDragDrop(const char* label, SharedPtr<Texture>& texture)
	{
		GUID oldTextureGUID = texture ? texture->GetGUID() : GUID{};

		bool hasSprite = texture != nullptr;
		ImGui::DragDropSelectable( label, hasSprite, hasSprite ? texture->GetPath().c_str() : "Null", TE_PAYLOAD_CONTENT_BROWSER_ITEM,
			[&]( const ImGuiPayload* payload )
			{
				std::string filePath = static_cast<const char*>( payload->Data );
				texture = AssetManager::GetAsset<Texture>( filePath );
			} );

		// On right click, give the option to remove the sprite, if there is one.
		if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
			ImGui::OpenPopup( label );
		if ( ImGui::BeginPopup( label ) )
		{
			if ( ImGui::MenuItem( "Remove Sprite", nullptr, nullptr, hasSprite ) )
				texture = nullptr;

			ImGui::EndPopup();
		}

		GUID newGUID = texture ? texture->GetGUID() : GUID{};
		return newGUID != oldTextureGUID;
	}

	void MaterialEditorPanel::OnImGuiDraw()
	{
		ImGuiWindowFlags flags = ( m_Material && m_Material->IsModified() ) ? ImGuiWindowFlags_UnsavedDocument : 0;
		if ( ImGuiBegin( flags ) && m_Material )
		{
			bool modified = m_Material->IsModified();
			modified |= ImGui::ColorEdit4( "Color", &m_Material->Color[0] );
			modified |= ImGui::SliderFloat( "Reflectivity", &m_Material->Reflectivity, 0.0f, 1.0f );
			modified |= ImGui::SliderFloat( "Refraction", &m_Material->Refraction, 0.0f, 1.0f );

			ImGui::Separator();

			modified |= DrawTextureDragDrop( "Base Color: ", m_Material->BaseColorTexture );
			modified |= DrawTextureDragDrop( "Normal Map: ", m_Material->NormalMapTexture );
			modified |= DrawTextureDragDrop( "Metallic: ", m_Material->MetallicTexture );
			modified |= DrawTextureDragDrop( "Roughness: ", m_Material->RoughnessTexture );
			modified |= DrawTextureDragDrop( "Emissive: ", m_Material->EmissiveTexture );

			m_Material->SetModified( modified );
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
				if ( m_Material && m_Material->IsModified() )
				{
					m_Material->Save();
					return true;
				}
			}
		}
		}

		return false;
	}

}

#endif // IS_EDITOR