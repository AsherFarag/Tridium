#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class PointLightComponent : public Component
	{
		REFLECT( PointLightComponent );
	public:
		PointLightComponent() = default;

		Vector3 Color = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
	};

	BEGIN_REFLECT_COMPONENT( PointLightComponent )
		PROPERTY( Color, FLAGS( Serialize, EditAnywhere ) )
		PROPERTY( Intensity, FLAGS( Serialize, EditAnywhere ) )
	END_REFLECT( PointLightComponent );
}