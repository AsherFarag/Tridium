#pragma once
#if IS_EDITOR
#include "Panel.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/Common/Function.h>

namespace Tridium {

	class OldInspectorPanel : public Panel
	{
	public:
		OldInspectorPanel();
		virtual ~OldInspectorPanel();

		virtual void OnImGuiDraw() override;

		void SetInspectedGameObject( OldGameObject gameObject );

	private:
		void DrawInspectedGameObject();
		void DrawComponents( OldGameObject a_GO );
		void DrawAddComponentButton();

		OldGameObject InspectedGameObject;
		DelegateHandle m_OnGameObjectSelectedHandle;
	};

}

#endif