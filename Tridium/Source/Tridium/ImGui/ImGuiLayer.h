#pragma once
#include <Tridium/Core/Layer.h>
#include <Tridium/Events/Eventsfwd.h>
#include "IconsFontAwesome6.h"

enum ImGuiKey;

namespace Tridium {

	class ImGuiLayer final : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiDraw() override;
		void Begin();
		void End();

	private:
		// Helper function that converts Tridiums Key Codes to ImGui's Key Codes
		ImGuiKey TridiumToImGuiKey( Input::KeyCode key );

	private:
		float m_Time = 0.f;
	};

}