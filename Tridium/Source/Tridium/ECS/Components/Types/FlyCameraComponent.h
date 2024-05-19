#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	DEFINE_COMPONENT( FlyCamera, ScriptableComponent )
	{
	public:
		virtual void OnUpdate() override {}

	protected:
		virtual void OnBegin() {}
		virtual void OnDestroy() {}
	};
}