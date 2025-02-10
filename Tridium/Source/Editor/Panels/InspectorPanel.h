#pragma once
#if IS_EDITOR
#include "Panel.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/Core/Delegate.h>

namespace Tridium {

	class InspectorPanel : public Panel
	{
	public:
		InspectorPanel();
		virtual ~InspectorPanel();

		virtual void OnImGuiDraw() override;

		void SetInspectedGameObject( GameObject gameObject );

	private:
		void DrawInspectedGameObject();
		void DrawAddComponentButton();

		GameObject InspectedGameObject;
		DelegateHandle m_OnGameObjectSelectedHandle;
	};

}

#endif