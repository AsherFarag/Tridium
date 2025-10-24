#include "InspectorPanel.h"

#if WITH_EDITOR

#include <Tridium/Editor/Editor.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/UI/PropertyDrawers.h>

namespace Tridium {

	struct Test
	{
		Vector3 Position;
		Vector3 Hidden;
		Vector3 Other;
		TransformComponent Transform;

		bool DisplayHidden() const { return false; }
		void TestFunction() { printf( std::format( "TestFunction called! Position = ({}, {}, {})\n", Position.X, Position.Y, Position.Z ).c_str() ); }
	};

	namespace Meta {

		template<>
		struct Reflector<Test>
		{
			Field<&Test::Position, Editable, Serializable>
			Position;

			Header HealthStats;

			Field<&Test::Hidden, Editable, Serializable, DisplayIf<&Test::DisplayHidden>>
			Hidden;

			Field<&Test::Transform, Editable, Serializable>
			Transform;

			Function<&Test::TestFunction, CallInEditor, DisplayName<"Execute Test Function">>
			TestFunction;

			Field<&Test::Other, Editable, Serializable>
			Other;
		};

	}

	void InspectorPanel::OnDraw( StringView a_Name, bool& o_Open )
	{
		if ( ImGui::Begin( a_Name.data(), &o_Open ) )
		{
			UI_DrawComponents();
		}

		ImGui::End();
	}

	void InspectorPanel::UI_DrawComponents()
	{
		static TransformComponent transform{};

		if ( UI::BeginTree( "Transform", UI::ETreeFlags::DefaultOpen | UI::ETreeFlags::Framed ) )
		{
			UIPropertyDrawer<TransformComponent>::Draw( "Transform", transform, false );
			UI::EndTree();
		}

		static Test test{};

		if ( UI::BeginTree( "Test Component", UI::ETreeFlags::DefaultOpen | UI::ETreeFlags::Framed ) )
		{
			UIPropertyDrawer<Test>::Draw( "Test Component", test, false );
			UI::EndTree();
		}
	}

} // namespace Tridium

#endif // WITH_EDITOR