#pragma once
#if IS_EDITOR
#include "Panel.h"
#include <Tridium/ECS/GameObject.h>

namespace Tridium::Editor {

	class InspectorPanel : public Panel
	{
	public:
		InspectorPanel() : Panel( "Inspector" ) {}

		virtual void OnImGuiDraw() override;

	public:
		GameObject InspectedGameObject;

	private:
		void DrawInspectedGameObject();
		void DrawAddComponentButton();
	};

}

#endif