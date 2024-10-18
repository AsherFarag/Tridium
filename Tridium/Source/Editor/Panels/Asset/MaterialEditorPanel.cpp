#include "tripch.h"
#ifdef IS_EDITOR
#include "Editor/EditorUtil.h"
#include "MaterialEditorPanel.h"
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/Rendering/Texture.h>

namespace Tridium::Editor {
	MaterialEditorPanel::MaterialEditorPanel()
		: Panel( "Material Editor##" )
	{
	}

	MaterialEditorPanel::MaterialEditorPanel( const AssetHandle& a_Material )
		: Panel( "Material Editor##" ), m_Material( a_Material )
	{
	}

	// Returns true if modified
	bool DrawTextureDragDrop(const char* label, AssetHandle& texture)
	{
		return false;
		//AssetHandle oldTextureHandle = texture.GetAssetHandle();

		//bool hasSprite = texture != nullptr;
		//ImGui::DragDropSelectable( label, hasSprite, hasSprite ? texture->GetPath().c_str() : "Null", TE_PAYLOAD_CONTENT_BROWSER_ITEM,
		//	[&]( const ImGuiPayload* payload )
		//	{
		//		std::string filePath = static_cast<const char*>( payload->Data );
		//		texture = AssetManager::GetAsset<Texture>( filePath );
		//	} );

		//// On right click, give the option to remove the sprite, if there is one.
		//if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
		//	ImGui::OpenPopup( label );
		//if ( ImGui::BeginPopup( label ) )
		//{
		//	if ( ImGui::MenuItem( "Remove Sprite", nullptr, nullptr, hasSprite ) )
		//		texture = nullptr;

		//	ImGui::EndPopup();
		//}

		//AssetHandle newHandle = texture ? texture.GetAssetHandle() : AssetHandle{};
		//return newHandle != oldTextureHandle;
	}

	void DrawMaterialProperty( const std::string& name, bool& wasModified, bool& wasRemoved )
	{
//		const bool open = ImGui::TreeNode( name.c_str() );
//
//		ImGui::SameLine( ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize( "Remove" ).x );
//		wasRemoved = ImGui::Button( "Remove" );
//
//		if ( open )
//		{
//			int currentPropertyType = (int)prop.Type;
//			const int numPropertyTypes = (int)Material::EPropertyType::None;
//			const char* const propertyTypes[numPropertyTypes] = { 
//				"Int", "IntArray",
//				"Float", "FloatArray",
//				"Color", "ColorArray",
//				"Vector4", "Vector4Array",
//				"Matrix4", "Matrix4Array",
//				"Texture",
//			};
//			wasModified |= ImGui::Combo( "Property Type", &currentPropertyType, propertyTypes, numPropertyTypes, -1 );
//
//			if ( wasModified )
//			{
//				prop.Type = (Material::EPropertyType)currentPropertyType;
//
//#define SET_PROP_TYPE(enumtype, type)\
//				case Material::EPropertyType::enumtype:\
//				{\
//					prop.Value = type{};\
//					break;\
//				}
//
//
//				switch ( prop.Type )
//				{
//					SET_PROP_TYPE( Int, int );
//					SET_PROP_TYPE( IntArray, std::vector<int> );
//					SET_PROP_TYPE( Float, float );
//					SET_PROP_TYPE( FloatArray, std::vector<float> );
//					SET_PROP_TYPE( Color, Color );
//					SET_PROP_TYPE( ColorArray, std::vector<Color> );
//					SET_PROP_TYPE( Vector4, Vector4 );
//					SET_PROP_TYPE( Vector4Array, std::vector<Vector4> );
//				default:
//					break;
//				}
//
//#undef SET_PROP_TYPE
//			}
//
//			// Forgive me
//
//#define IMGUI_DRAW_PROPERTY(enumType, drawFunc, type, mod)\
//						case Material::EPropertyType::enumType:\
//						{\
//							ImGui::drawFunc("##v", &(std::get<type>( prop.Value ))mod );\
//							break;\
//						}
//
//#define IMGUI_DRAW_ARRAY_PROPERTY(enumType, drawFunc, type, mod)\
//			case Material::EPropertyType::enumType:\
//			{\
//				std::vector<type>& arr = std::get<std::vector<type>>( prop.Value );\
//				for ( int i = 0; i < arr.size(); ++i )\
//				{\
//					wasModified |= ImGui::drawFunc( std::to_string( i ).c_str(), &(arr[i])mod );\
//					ImGui::SameLine();\
//					if ( ImGui::Button( "Remove" ) )\
//					{\
//						wasModified = true;\
//						arr.erase( arr.begin() + i );\
//						break;\
//					}\
//				}\
//				if ( ImGui::Button( "Add" ) )\
//					arr.emplace_back( type{} );\
//				break;\
//			}
//
//			switch ( prop.Type )
//			{
//				IMGUI_DRAW_PROPERTY( Int, DragInt, int );
//				IMGUI_DRAW_ARRAY_PROPERTY( IntArray, DragInt, int );
//				IMGUI_DRAW_PROPERTY( Float, DragFloat, float );
//				IMGUI_DRAW_ARRAY_PROPERTY( FloatArray, DragFloat, float );
//				IMGUI_DRAW_PROPERTY( Color, ColorEdit4, Color, [0]);
//				IMGUI_DRAW_ARRAY_PROPERTY( ColorArray, ColorEdit4, Color, [0]);
//				IMGUI_DRAW_PROPERTY( Vector4, DragFloat4, Vector4, [0] );
//				IMGUI_DRAW_ARRAY_PROPERTY( Vector4Array, DragFloat4, Vector4, [0] );
//			}
//
//#undef IMGUI_DRAW_PROPERTY
//#undef IMGUI_DRAW_ARRAY_PROPERTY
//
//			ImGui::TreePop();
//		}
	}

	void MaterialEditorPanel::OnImGuiDraw()
	{
		//ImGuiWindowFlags flags = ( m_Material && m_Modified) ? ImGuiWindowFlags_UnsavedDocument : 0;
		//if ( ImGuiBegin( flags ) && m_Material )
		//{
		//	if ( ImGui::TreeNode( "Properties" ) )
		//	{
		//		Material::PropertyTable& properties = m_Material->GetProperties();
		//		for ( auto& [name, prop] : properties )
		//		{
		//			bool wasRemoved = false;
		//			DrawMaterialProperty( name, prop, m_Modified, wasRemoved );

		//			if ( wasRemoved )
		//			{
		//				properties.erase( name );
		//				break;
		//			}
		//		}

		//		if ( ImGui::Button( "Add property" ) )
		//		{
		//			m_Modified = true;

		//			bool nameExists = false;
		//			std::string name;
		//			int index = 0;
		//			do
		//			{
		//				name = "NewProperty";
		//				name.append( std::to_string(index) );
		//				nameExists = properties.find( name ) != properties.end();
		//				index++;
		//			} while ( nameExists );


		//			properties.insert( { name, {} } );
		//		}

		//		ImGui::TreePop();
		//	}
		//}

		//m_IsHovered = ImGui::IsWindowHovered();
		//m_IsFocused = ImGui::IsWindowFocused();

		//ImGuiEnd();
	}

	bool MaterialEditorPanel::OnKeyPressed( KeyPressedEvent& e )
	{
		//if ( e.IsRepeat() )
		//	return false;

		//bool control = Input::IsKeyPressed( Input::KEY_LEFT_CONTROL );

		//switch ( e.GetKeyCode() )
		//{
		//case Input::KEY_S:
		//{
		//	if ( control )
		//	{
		//		if ( m_Material )
		//		{
		//			//MaterialSerializer s( m_Material );
		//			//s.SerializeText(AssetManager::Ge);
		//			return true;
		//		}
		//	}
		//}
		//}

		return false;
	}

}

#endif // IS_EDITOR