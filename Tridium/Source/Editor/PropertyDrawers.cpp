#include "tripch.h"
#ifdef IS_EDITOR

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
		return ImGui::InputText( a_Name, &a_Value, ImGuiInputTextFlags_EnterReturnsTrue );
	}

#define _DRAW_FLOAT(_id, _color) \
	ImGui::PushStyleColor(ImGuiCol_Border, _color.Value); \
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
	bool DrawProperty( const char* a_Name, AssetHandle& a_Value, EDrawPropertyFlags a_Flags )
	{
		IS_DISABLED( a_Flags );
		return ImGui::InputScalar(
			a_Name, ImGuiDataType_U64, &a_Value,
			nullptr, nullptr, nullptr,
			ImGuiInputTextFlags_EnterReturnsTrue);
	}

#undef _DRAW_FLOAT

}
#endif // IS_EDITOR