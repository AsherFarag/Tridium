#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	DEFINE_COMPONENT( SphereColliderComponent )
	{
	public:
		bool IsColliding = false;
		float Radius = 0.5f;
	};

}