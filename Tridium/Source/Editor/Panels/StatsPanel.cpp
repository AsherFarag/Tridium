#include "tripch.h"
#if IS_EDITOR

#include "StatsPanel.h"

#include <Editor/EditorStyle.h>
#include <Tridium/Core/Application.h>
#include <Tridium/Scene/Scene.h>
#include <Tridium/Graphics/Rendering/SceneRenderer.h>

#define STAT_UNIT "%.3f ms"

namespace Tridium::Editor {

	StatsPanel::StatsPanel()
		: Panel( "Stats" )
	{
	}

	void StatsPanel::OnImGuiDraw()
	{
		if ( !ImGuiBegin() )
		{
			ImGuiEnd();
			return;
		}

		// Stats
		{
			// FPS
			{
				ImVec4 fpsNumberColor;
				if ( Application::Get().GetFPS() < 30 )
					fpsNumberColor = Style::Colors::Red;
				else if ( Application::Get().GetFPS() < 60 )
					fpsNumberColor = Style::Colors::Yellow;
				else
					fpsNumberColor = Style::Colors::Green;

				ImGui::Text( "FPS: " );
				ImGui::SameLine();
				ImGui::TextColored( fpsNumberColor, "%d", Application::Get().GetFPS() );

				ImGui::Text( "Frame Time: " );
				ImGui::SameLine();
				ImGui::TextColored( fpsNumberColor, STAT_UNIT, Application::Get().GetFrameTime() );

				ImGui::Text( "Min FPS: %d", Application::Get().GetFrameInfo().MinFPS );
				ImGui::Text( "Max FPS: %d", Application::Get().GetFrameInfo().MaxFPS );

				ImGui::Separator();
			}

			// Render Stats
			if ( SharedPtr<Scene> scene = Application::GetScene() )
			{
				const RenderStats& renderStats = scene->GetSceneRenderer().GetRenderStats();

				ImGui::Text( "Render Stats" );

				ImGui::Text( "Render Time: " STAT_UNIT, renderStats.RenderTime );
				ImGui::Text( "	- Geometry Time: " STAT_UNIT, renderStats.GeometryTime );
				ImGui::Text( "	- Shadow Map Time: " STAT_UNIT, renderStats.ShadowMapTime );
				ImGui::Text( "	- Post Process Time: " STAT_UNIT, renderStats.PostProcessTime );
				ImGui::Text( "	- Draw List Generation Time: " STAT_UNIT, renderStats.DrawListGenerationTime );
				ImGui::Text( "Draw Call Count : %d", renderStats.NumDrawCalls );
				ImGui::Text( "Culled Draw Calls: %d", renderStats.CulledDrawCalls );

				ImGui::Separator();
			}
		}

		ImGuiEnd();
	}
}
#endif // IS_EDITOR