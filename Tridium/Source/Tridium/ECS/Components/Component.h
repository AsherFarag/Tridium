#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/ECS/GameObject.h>
#include <Tridium/Reflection/ReflectionFwd.h>

namespace Tridium {

	class Component
	{
		REFLECT( Component );
	public:
		Component();
		~Component() = default;

		GameObject GetGameObject() const { return m_GameObject; }

	private:
		GameObject m_GameObject;

		friend class Scene;
		friend class GameObject;
	};

	class NativeScriptComponent : public Component
	{
		REFLECT( NativeScriptComponent );
	public:
		NativeScriptComponent() = default;
		~NativeScriptComponent() = default;

		// Called before the first frame or when this component is created.
		void OnBeginPlay() {}
		// Called every frame.
		void OnUpdate( float a_DeltaTime ) {}
		// Called when this component is destroyed.
		void OnDestroy() {}

		const Scene* GetScene() const { return Application::GetScene().get(); }

		template<typename T>
		bool HasComponent() const { return m_GameObject.HasComponent<T>(); }

		template<typename T>
		T* TryGetComponent() const { return m_GameObject.TryGetComponent<T>(); }

		template<typename T>
		T& GetComponent() const { return m_GameObject.GetComponent<T>(); }

		template<typename T, typename... _Args>
		T& AddComponent( _Args&&... a_Args ) const { return m_GameObject.AddComponent<T>( std::forward<_Args>( a_Args )... ); }

		template<typename T>
		void RemoveComponent() const { m_GameObject.RemoveComponent<T>(); }

	private:
		friend class Scene;
		friend class GameObject;
	};
}