#pragma once
#include <Tridium/ECS/Components/Component.h>

namespace Tridium
{
	class TestComponent :
		public Component
	{
		template<typename T> friend class ::Tridium::Refl::Reflector;
	public:

		TestComponent() {}
		virtual ~TestComponent() = default;
		virtual void OnDestroy() {}

		inline GUID GetID() const { return m_ID; }
	private:
		GUID m_ID;
	};
}

//template<> struct ::Tridium::Refl::Reflector<::Tridium::TestComponent>;

