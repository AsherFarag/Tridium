#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium {

	class TransformComponent;

	DEFINE_COMPONENT( FlyCameraComponent, ScriptableComponent )
	{
	public:
		virtual void OnUpdate() override;

	public:
		float Sensitivity = 1.f;
		float Speed = 3.f;

	protected:
		virtual void OnConstruct() override;
		virtual void OnDestroy() override;

	private:
		TransformComponent* m_Transform;

		Vector2 m_LastMousePos;
	};
}