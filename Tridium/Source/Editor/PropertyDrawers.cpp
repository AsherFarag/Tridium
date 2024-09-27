#include "tripch.h"
#include "PropertyDrawers.h"

namespace Tridium::Editor {

#define IS_DISABLED(Flags) Internal::ScopedDisable _DISABLED_{ Flags == EDrawPropertyFlags::ReadOnly }

	template<>
	bool DrawProperty( const char* a_Name, bool& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::Checkbox( a_Name, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, int& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragInt( a_Name, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, float& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::DragFloat( a_Name, &a_Value );
	}

	template<>
	bool DrawProperty( const char* a_Name, std::string& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::InputText( a_Name, a_Value.data(), a_Value.size() );
	}

#define _DRAW_FLOAT(_id, _color) \
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(_color)); \
	ImGui::PushID(_id); \
	modified |= ImGui::DragFloat("", &a_Value[_id]); \
	ImGui::PopID(); \
	ImGui::PopStyleColor();

	template<>
	bool DrawProperty( const char* a_Name, Vector2& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );

		bool modified = false;
		{
			float itemWidth = ImGui::GetContentRegionAvail().x / 3.f - 30;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

			ImGui::BeginGroup();
			ImGui::PushID( a_Name );
			{
				ImGui::PushItemWidth( itemWidth );

				// x
				_DRAW_FLOAT( 0, EditorStyle::Red )

					ImGui::SameLine();

				// y
				_DRAW_FLOAT( 1, EditorStyle::Green )

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
			float itemWidth = ImGui::GetContentRegionAvail().x / 3.f - 30;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

			ImGui::BeginGroup();
			ImGui::PushID( a_Name );
			{
				ImGui::PushItemWidth( itemWidth );

				// x
				_DRAW_FLOAT( 0, EditorStyle::Red )

					ImGui::SameLine();

				// y
				_DRAW_FLOAT( 1, EditorStyle::Green )

					ImGui::SameLine();

				// z
				_DRAW_FLOAT( 2, EditorStyle::Blue )

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
			float itemWidth = ImGui::GetContentRegionAvail().x / 3.f - 30;

			ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );

			ImGui::BeginGroup();
			ImGui::PushID( a_Name );
			{
				ImGui::PushItemWidth( itemWidth );

				// x
				_DRAW_FLOAT( 0, EditorStyle::Red )

					ImGui::SameLine();

				// y
				_DRAW_FLOAT( 1, EditorStyle::Green )

					ImGui::SameLine();

				// z
				_DRAW_FLOAT( 2, EditorStyle::Blue )

					ImGui::SameLine();

				// w
				_DRAW_FLOAT( 3, EditorStyle::Orange )

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

#undef _DRAW_FLOAT

}
