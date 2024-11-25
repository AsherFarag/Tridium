#include "tripch.h"
#ifdef IS_EDITOR

#include "PropertyDrawers.h"

#include <Tridium/Asset/EditorAssetManager.h>

#include <Tridium/Rendering/Texture.h>

#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types.h>

#include <Tridium/Rendering/Framebuffer.h>

namespace Tridium::Editor {

#define IS_DISABLED(Flags) Internal::ScopedDisable _DISABLED_{ Flags == EDrawPropertyFlags::ReadOnly }

	template<>
	bool DrawProperty( const char* a_Name, bool& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::Checkbox( a_Name, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, int8_t& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragScalar( a_Name, ImGuiDataType_S8, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, int16_t& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragScalar( a_Name, ImGuiDataType_S16, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, int32_t& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragScalar( a_Name, ImGuiDataType_S32, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, int64_t& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragScalar( a_Name, ImGuiDataType_S64, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, uint8_t& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragScalar( a_Name, ImGuiDataType_U8, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, uint16_t& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragScalar( a_Name, ImGuiDataType_U16, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, uint32_t& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragScalar( a_Name, ImGuiDataType_U32, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, uint64_t& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragScalar( a_Name, ImGuiDataType_U64, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, float& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragFloat( a_Name, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, double& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragScalar( a_Name, ImGuiDataType_Double, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, std::string& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::InputText( a_Name, &a_Value, ImGuiInputTextFlags_EnterReturnsTrue );
	}

#define _DRAW_FLOAT(_id, _color) \
	ImGui::PushStyleColor(ImGuiCol_Border, _color.Value); \
	ImGui::PushID(_id); \
	modified |= ImGui::DragFloat("", &a_Value[_id]); \
	ImGui::PopID(); \
	ImGui::PopStyleColor();

#define _DRAW_INT(_id, _color) \
	ImGui::PushStyleColor(ImGuiCol_Border, _color.Value); \
	ImGui::PushID(_id); \
	modified |= ImGui::DragInt("", &a_Value[_id]); \
	ImGui::PopID(); \
	ImGui::PopStyleColor();

	template<>
	bool DrawProperty( const char* a_Name, Vector2& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );

		bool modified = false;
		{
			constexpr float NumFloats = 2;
			float regionWidthAvail = ImGui::GetContentRegionAvail().x - ( ImGui::GetContentRegionMax().x - ImGui::GetContentRegionAvail().x );
			regionWidthAvail -= ImGui::CalcTextSize( a_Name ).x;
			regionWidthAvail -= ImGui::GetStyle().ItemSpacing.x * NumFloats - 1;
			float itemWidth = regionWidthAvail / NumFloats;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

			ImGui::BeginGroup();
			ImGui::PushID( a_Name );
			{
				ImGui::PushItemWidth( itemWidth );

				// x
				_DRAW_FLOAT( 0, Style::Colors::Red )

					ImGui::SameLine();

				// y
				_DRAW_FLOAT( 1, Style::Colors::Green )

					ImGui::PopItemWidth();
			}
			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::PopStyleVar();

			ImGui::SameLine();

			ImGui::Text( a_Name );
		}

		return modified;
	}

	template<>
	bool DrawProperty( const char* a_Name, Vector3& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );

		bool modified = false;
		{
			constexpr float NumFloats = 3;
			float regionWidthAvail = ImGui::GetContentRegionAvail().x - ( ImGui::GetContentRegionMax().x - ImGui::GetContentRegionAvail().x );
			regionWidthAvail -= ImGui::CalcTextSize( a_Name ).x;
			regionWidthAvail -= ImGui::GetStyle().ItemSpacing.x * NumFloats - 1;
			float itemWidth = regionWidthAvail / NumFloats;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

			ImGui::BeginGroup();
			ImGui::PushID( a_Name );
			{
				ImGui::PushItemWidth( itemWidth );

				// x
				_DRAW_FLOAT( 0, Style::Colors::Red )

					ImGui::SameLine();

				// y
				_DRAW_FLOAT( 1, Style::Colors::Green )

					ImGui::SameLine();

				// z
				_DRAW_FLOAT( 2, Style::Colors::Blue )

					ImGui::PopItemWidth();
			}
			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::PopStyleVar();

			ImGui::SameLine();
			ImGui::Text( a_Name );
		}

		return modified;
	}

	template<>
	bool DrawProperty( const char* a_Name, Vector4& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );

		bool modified = false;
		{
			constexpr float NumFloats = 4;
			float regionWidthAvail = ImGui::GetContentRegionAvail().x - ( ImGui::GetContentRegionMax().x - ImGui::GetContentRegionAvail().x );
			regionWidthAvail -= ImGui::CalcTextSize( a_Name ).x;
			regionWidthAvail -= ImGui::GetStyle().ItemSpacing.x * NumFloats - 1;
			float itemWidth = regionWidthAvail / NumFloats;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

			ImGui::BeginGroup();
			ImGui::PushID( a_Name );
			{
				ImGui::PushItemWidth( itemWidth );

				// x
				_DRAW_FLOAT( 0, Style::Colors::Red )

					ImGui::SameLine();

				// y
				_DRAW_FLOAT( 1, Style::Colors::Green )

					ImGui::SameLine();

				// z
				_DRAW_FLOAT( 2, Style::Colors::Blue )

					ImGui::SameLine();

				// w
				_DRAW_FLOAT( 3, Style::Colors::Orange )

					ImGui::PopItemWidth();
			}
			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::PopStyleVar();

			ImGui::SameLine();
			ImGui::Text( a_Name );
		}

		return modified;
	}

	template<>
	bool DrawProperty( const char* a_Name, iVector2& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );

		bool modified = false;
		{
			constexpr float NumInts = 2;
			float regionWidthAvail = ImGui::GetContentRegionAvail().x - ( ImGui::GetContentRegionMax().x - ImGui::GetContentRegionAvail().x );
			regionWidthAvail -= ImGui::CalcTextSize( a_Name ).x;
			regionWidthAvail -= ImGui::GetStyle().ItemSpacing.x * NumInts - 1;
			float itemWidth = regionWidthAvail / NumInts;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

			ImGui::BeginGroup();
			ImGui::PushID( a_Name );
			{
				ImGui::PushItemWidth( itemWidth );

				// x
				_DRAW_INT( 0, Style::Colors::Red )

					ImGui::SameLine();

				// y
				_DRAW_INT( 1, Style::Colors::Green )

					ImGui::PopItemWidth();
			}
			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::PopStyleVar();

			ImGui::SameLine();
			ImGui::Text( a_Name );
		}

		return modified;
	}

	template<>
	bool DrawProperty( const char* a_Name, iVector3& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );

		bool modified = false;
		{
			constexpr float NumInts = 3;
			float regionWidthAvail = ImGui::GetContentRegionAvail().x - ( ImGui::GetContentRegionMax().x - ImGui::GetContentRegionAvail().x );
			regionWidthAvail -= ImGui::CalcTextSize( a_Name ).x;
			regionWidthAvail -= ImGui::GetStyle().ItemSpacing.x * NumInts - 1;
			float itemWidth = regionWidthAvail / NumInts;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

			ImGui::BeginGroup();
			ImGui::PushID( a_Name );
			{
				ImGui::PushItemWidth( itemWidth );

				// x
				_DRAW_INT( 0, Style::Colors::Red )

					ImGui::SameLine();

				// y
				_DRAW_INT( 1, Style::Colors::Green )

					ImGui::SameLine();

				// z
				_DRAW_INT( 2, Style::Colors::Blue )

					ImGui::PopItemWidth();
			}
			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::PopStyleVar();

			ImGui::SameLine();
			ImGui::Text( a_Name );
		}

		return modified;
	}

	template<>
	bool DrawProperty( const char* a_Name, iVector4& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );

		bool modified = false;
		{
			constexpr float NumInts = 4;
			float regionWidthAvail = ImGui::GetContentRegionAvail().x - ( ImGui::GetContentRegionMax().x - ImGui::GetContentRegionAvail().x );
			regionWidthAvail -= ImGui::CalcTextSize( a_Name ).x;
			regionWidthAvail -= ImGui::GetStyle().ItemSpacing.x * NumInts - 1;
			float itemWidth = regionWidthAvail / NumInts;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

			ImGui::BeginGroup();
			ImGui::PushID( a_Name );
			{
				ImGui::PushItemWidth( itemWidth );

				// x
				_DRAW_INT( 0, Style::Colors::Red )

					ImGui::SameLine();

				// y
				_DRAW_INT( 1, Style::Colors::Green )

					ImGui::SameLine();

				// z
				_DRAW_INT( 2, Style::Colors::Blue )

					ImGui::SameLine();

				// w
				_DRAW_INT( 3, Style::Colors::Orange )

					ImGui::PopItemWidth();
			}
			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::PopStyleVar();

			ImGui::SameLine();
			ImGui::Text( a_Name );
		}

		return modified;
	}

	template<>
	bool DrawProperty( const char* a_Name, Rotator& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		Vector3 euler = glm::degrees( a_Value.Euler );
		if ( DrawProperty( a_Name, euler, a_Flags ) )
		{
			a_Value.SetFromEuler( glm::radians( euler ) );
			return true;
		}

		return false;
	}

	template<>
	bool DrawProperty( const char* a_Name, Color& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::ColorEdit4( a_Name, &a_Value.r );
	}

	//////////////////////////////////////////////////////////////////////////
	// AssetHandle
	//////////////////////////////////////////////////////////////////////////

	template <EAssetType _AssetType>
	bool _DrawAssetHandleProperty( const char* a_Name, AssetHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		AssetMetaData assetMetaData = EditorAssetManager::Get()->GetAssetMetaData(a_Value);
		const char* assetName = "None";
		if ( assetMetaData.IsValid() )
			assetName = assetMetaData.Name.c_str();

		bool modified = false;

		IS_DISABLED( a_Flags );
		const bool open = ImGui::BeginCombo( a_Name, assetName );

		ImGui::ScopedDragDropTarget scopedDragDropTarget;
		if ( scopedDragDropTarget )
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( TE_PAYLOAD_ASSET_HANDLE, ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceAllowNullID );
			if ( payload )
			{
				AssetHandle assetHandle( *(AssetHandle*)payload->Data );
				const AssetMetaData& assetMetaData = EditorAssetManager::Get()->GetAssetMetaData( assetHandle );
				if ( assetMetaData.IsValid() && assetMetaData.AssetType == _AssetType )
				{
					a_Value = assetHandle;
					return true;
				}
			}
		}

		if ( open )
		{
			if ( ImGui::Selectable( "None###Internal", a_Value == AssetHandle::InvalidGUID ) )
			{
				a_Value = AssetHandle::InvalidGUID;
				ImGui::EndCombo();
				return true;
			}

			ImGui::Separator();

			for ( const auto& [handle, assetMetaData] : EditorAssetManager::Get()->GetAssetRegistry().AssetMetaData )
			{
				if ( assetMetaData.AssetType != _AssetType )
					continue;


				std::string name = !assetMetaData.Name.empty() ? assetMetaData.Name : assetMetaData.Path.ToString();
				ImGui::ScopedID id( handle.ID() );
				bool selected = a_Value == handle;
				if ( ImGui::Selectable( name.c_str(), selected ) && !selected )
				{
					a_Value = handle;
					modified = true;
					break;
				}

				if ( ImGui::BeginItemTooltip() )
				{
					ImGui::Text( "Asset Type: %s", AssetTypeToString( assetMetaData.AssetType ) );
					ImGui::Text( "Path: %s", assetMetaData.Path.ToString().c_str() );

					ImGui::EndTooltip();
				}
			}

			ImGui::EndCombo();
		}

		return modified;
	}

	template<>
	bool DrawProperty( const char* a_Name, AssetHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		AssetMetaData assetMetaData = EditorAssetManager::Get()->GetAssetMetaData( a_Value );
		const char* assetName = "None";
		if ( assetMetaData.IsValid() )
			assetName = assetMetaData.Name.c_str();

		bool modified = false;

		IS_DISABLED( a_Flags );

		const bool open = ImGui::BeginCombo( a_Name, assetName );

		ImGui::ScopedDragDropTarget scopedDragDropTarget;
		if ( scopedDragDropTarget )
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( TE_PAYLOAD_ASSET_HANDLE, ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceAllowNullID );
			if ( payload )
			{
				AssetHandle assetHandle( *(AssetHandle*)payload->Data );
				const AssetMetaData& assetMetaData = EditorAssetManager::Get()->GetAssetMetaData( assetHandle );
				if ( assetMetaData.IsValid() )
				{
					a_Value = assetHandle;
					return true;
				}
			}
		}

		if ( open )
		{
			if ( ImGui::Selectable( "None###Internal", a_Value == AssetHandle::InvalidGUID ) )
			{
				a_Value = AssetHandle::InvalidGUID;
				ImGui::EndCombo();
				return true;
			}

			ImGui::Separator();

			for ( const auto& [handle, assetMetaData] : EditorAssetManager::Get()->GetAssetRegistry().AssetMetaData )
			{
				std::string name = !assetMetaData.Name.empty() ? assetMetaData.Name : assetMetaData.Path.ToString();
				ImGui::ScopedID id( handle.ID() );
				if ( ImGui::Selectable( name.c_str(), a_Value == handle ) )
				{
					a_Value = handle;
					modified = true;
					break;
				}

				if ( ImGui::BeginItemTooltip() )
				{
					ImGui::Text( "Asset Type: %s", AssetTypeToString( assetMetaData.AssetType ) );
					ImGui::Text( "Path: %s", assetMetaData.Path.ToString().c_str() );

					ImGui::EndTooltip();
				}
			}

			ImGui::EndCombo();
		}

		return modified;
	}

	template<>
	bool DrawProperty( const char* a_Name, SceneHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		return _DrawAssetHandleProperty<EAssetType::Scene>( a_Name, a_Value, a_Flags );
	}

	template<>
	bool DrawProperty( const char* a_Name, MaterialHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		return _DrawAssetHandleProperty<EAssetType::Material>( a_Name, a_Value, a_Flags );
	}

	template<>
	bool DrawProperty( const char* a_Name, MeshSourceHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		return _DrawAssetHandleProperty<EAssetType::MeshSource>( a_Name, a_Value, a_Flags );
	}

	template<>
	bool DrawProperty( const char* a_Name, StaticMeshHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		return _DrawAssetHandleProperty<EAssetType::StaticMesh>( a_Name, a_Value, a_Flags );
	}

	template<>
	bool DrawProperty( const char* a_Name, ShaderHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		return _DrawAssetHandleProperty<EAssetType::Shader>( a_Name, a_Value, a_Flags );
	}

	template<>
	bool DrawProperty( const char* a_Name, TextureHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		return _DrawAssetHandleProperty<EAssetType::Texture>( a_Name, a_Value, a_Flags );
	}

	template<>
	bool DrawProperty( const char* a_Name, CubeMapHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		return _DrawAssetHandleProperty<EAssetType::CubeMap>( a_Name, a_Value, a_Flags );
	}

	template<>
	bool DrawProperty( const char* a_Name, LuaHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		return _DrawAssetHandleProperty<EAssetType::Lua>( a_Name, a_Value, a_Flags );
	}

	template<>
	bool DrawProperty( const char* a_Name, GameObject& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );

		bool modified = false;

		// Get the name of the GameObject
		const char* gameObjectName = "None";
		if ( a_Value.IsValid() )
		{
			if ( TagComponent* tag = a_Value.TryGetComponent<TagComponent>() )
			{
				gameObjectName = tag->Tag.c_str();
			}
		}


		const bool open = ImGui::BeginCombo( a_Name, gameObjectName );

		ImGui::ScopedDragDropTarget scopedDragDropTarget;
		if ( scopedDragDropTarget )
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( TE_PAYLOAD_GAME_OBJECT, ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceAllowNullID );
			if ( payload )
			{
				GameObject gameObject( *(GameObject*)payload->Data );
				modified |= gameObject != a_Value;
				a_Value = gameObject;
			}
		}

		if ( open )
		{
			if ( ImGui::Selectable( "None###Internal", !a_Value.IsValid() ) )
			{
				a_Value = {};
				ImGui::EndCombo();
				return true;
			}

			ImGui::Separator();

			auto view = Application::GetScene()->GetRegistry().view<TagComponent>();
			view.each( [&]( const entt::entity& entity, TagComponent& tag )
				{
					GameObject gameObject( entity );
					ImGui::ScopedID id( gameObject );
					if ( ImGui::Selectable( tag.Tag.c_str(), gameObject == a_Value ) )
					{
						a_Value = gameObject;
						modified = true;
					}
				} );

			ImGui::EndCombo();
		}

		return modified;
	}

	//////////////////////////////////////////////////////////////////////////

	template<>
	bool DrawProperty( const char* a_Name, SharedPtr<Framebuffer>& a_Value, EDrawPropertyFlags a_Flags )
	{
		if ( ImGui::TreeNode(a_Name) )
		{
			if ( a_Value )
			{
				if ( a_Value->GetDepthAttachmentID() )
				{
					ImGui::Image( (ImTextureID)a_Value->GetDepthAttachmentID(), ImVec2( 128, 128 ), ImVec2( 0, 1 ), ImVec2( 1, 0 ) );
				}
			}

			ImGui::TreePop();
		}

		return false;
	}

#undef _DRAW_FLOAT

}
#endif // IS_EDITOR