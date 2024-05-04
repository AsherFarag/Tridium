#pragma once

#include <Tridium/Core/Core.h>
#include <Tridium/Events/Event.h>

namespace Tridium {

	class TRIDIUM_API Layer
	{
	public:
		Layer( const std::string& name = "Layer" );
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}

		const auto& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};

}


