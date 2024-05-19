#pragma once
#include <Tridium/Core/Core.h>
#include "entt.hpp"

namespace Tridium {

	// - Type Definitions -


#define DEFINE_BASE_COMPONENT( x ) class x
#define DEFINE_INHERITED_COMPONENT( x, y ) class x : public y

#define DEFINE_COMPONENT(...) EXPAND(SELECT_MACRO_2( __VA_ARGS__, DEFINE_INHERITED_COMPONENT, DEFINE_BASE_COMPONENT )(__VA_ARGS__))

	DEFINE_COMPONENT( ScriptableComponent )
	{
	public:
		ScriptableComponent()
		{
			OnBegin();
		}
		~ScriptableComponent()
		{
			OnDestroy();
		}

		virtual void OnUpdate() {}

	protected:
		virtual void OnBegin() {}
		virtual void OnDestroy() {}
	};
}