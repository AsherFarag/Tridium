#pragma once
#include <Tridium/Core/Core.h>
#include <Tridium/ECS/ECS.h>
#include <Tridium/Core/Enum.h>
#include <Tridium/Reflection/ReflectionFwd.h>

//================================================================
// COMPONENT_BODY Macro
//  Used for defining the body of a component and setting up 
//  common functionality for components.
//  Usage:
//   class MyComponent : public Component
//   {
//      COMPONENT_BODY( MyComponent, Component );
//   public:
//      // Your code here
//   };
//================================================================
#define COMPONENT_BODY( _Class, _Parent ) \
	private:                              \
		REFLECT( _Class );	              \
	public:                               \
		using Super = _Parent;            \
		friend class ::Tridium::Scene;	  \
		friend class ::Tridium::GameObject; 

namespace Tridium {		  

	//================================================================
	// Component
	//  Base class for all components.
	//  Comes with no functionality.
	//  Use for simple data components that do not require scripting
	//  or custom functionality.
	//================================================================
	class Component
	{
		REFLECT( Component );
		friend class Scene;
		friend class GameObject;
	public:
		Component();
		~Component() = default;

		GameObject GetGameObject() const;

	private:
		EntityID m_EntityID;
	};

	//================================================================
	// Component Flags
	//  Bit flags used to represent the state of a component.
	enum class EComponentFlags : uint32_t
	{
		None = 0,
		// If not active, this component will not be updated.
		Active = 1 << 1,
		// If not visible, this component will not be rendered.
		Visible = 1 << 2,
	};
	ENUM_ENABLE_BITMASK_OPERATORS( EComponentFlags );
	//================================================================

} // namespace Tridium