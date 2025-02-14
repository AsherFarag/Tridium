#include "tripch.h"
#if IS_EDITOR
#include "MaterialEditorPanel.h"
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Graphics/Rendering/Texture.h>
#include <Tridium/Asset/EditorAssetManager.h>
#include <Tridium/Editor/PropertyDrawers.h>
#include <Tridium/Editor/EditorUtil.h>

namespace Tridium {
	MaterialEditorPanel::MaterialEditorPanel()
		: Panel( "Material Editor##" )
	{
	}

	MaterialEditorPanel::MaterialEditorPanel( const MaterialHandle& a_Material )
		: Panel( "Material Editor##" ), m_Material( a_Material )
	{
	}

	void MaterialEditorPanel::OnImGuiDraw()
	{
		SharedPtr<Material> material = AssetManager::GetAsset<Material>( m_Material );
		ImGuiWindowFlags flags = ( material && m_Modified) ? ImGuiWindowFlags_UnsavedDocument : 0;
		if ( ImGuiBegin( flags ) )
		{
			if ( Editor::DrawProperty( "Material", m_Material, EDrawPropertyFlags::Editable ) )
			{
				m_Modified = false;
			}

			ImGui::Separator();

			if ( material )
			{
				m_Modified |= ImGui::ColorEdit3( "Albedo Color", &material->AlbedoColor[0] );
				m_Modified |= ImGui::SliderFloat( "Metallic Intensity", &material->MetallicIntensity, 0.0f, 1.0f );
				m_Modified |= ImGui::SliderFloat( "Roughness Intensity", &material->RoughnessIntensity, 0.0f, 1.0f );
				m_Modified |= ImGui::SliderFloat( "Emissive Intensity", &material->EmissiveIntensity, 0.0f, 1.0f );

				ImGui::Separator();

				m_Modified |= Editor::DrawProperty( "Albedo Texture", material->AlbedoTexture, EDrawPropertyFlags::Editable );
				m_Modified |= Editor::DrawProperty( "Metallic Texture", material->MetallicTexture, EDrawPropertyFlags::Editable );
				m_Modified |= Editor::DrawProperty( "Roughness Texture", material->RoughnessTexture, EDrawPropertyFlags::Editable );
				m_Modified |= Editor::DrawProperty( "Normal Texture", material->NormalTexture, EDrawPropertyFlags::Editable );
				m_Modified |= Editor::DrawProperty( "Opacity Texture", material->OpacityTexture, EDrawPropertyFlags::Editable );
				m_Modified |= Editor::DrawProperty( "Emissive Texture", material->EmissiveTexture, EDrawPropertyFlags::Editable );
				m_Modified |= Editor::DrawProperty( "AO Texture", material->AOTexture, EDrawPropertyFlags::Editable );
			}
		}

		m_IsHovered = ImGui::IsWindowHovered();
		m_IsFocused = ImGui::IsWindowFocused();

		ImGuiEnd();

		if ( !m_Open )
		{
			OnClose();
		}
	}

	bool MaterialEditorPanel::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat() )
			return false;

		bool control = Input::IsKeyPressed( EInputKey::LeftControl );

		switch ( e.GetKeyCode() )
		{
			case EInputKey::S:
			{
				if ( control && m_Material.IsValid() )
				{
					AssetManager::Get<EditorAssetManager>()->SaveAsset( m_Material );
					m_Modified = false;
					return true;
				}
			}
		}

		return false;
	}

	void MaterialEditorPanel::SetMaterial( MaterialHandle a_Material )
	{
		m_Material = a_Material;
		m_Modified = false;
	}

}

#endif // IS_EDITOR