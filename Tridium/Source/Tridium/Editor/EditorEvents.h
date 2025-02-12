#pragma once
#ifdef IS_EDITOR
#include <Tridium/Core/Delegate.h>
#include <Tridium/Core/Types.h>

namespace Tridium {

	// Forward Declarations
	class GameObject;

	//=================================================================================================
	// Editor Events
	//  The global editor event system.
	//  This namespace contains global events that can be subscribed in the editor.
	namespace Editor::Events {

		extern MulticastDelegate<void, GameObject> OnGameObjectSelected;

	} // namespace Editor::Events
	//=================================================================================================

} // namespace Tridium

#endif // IS_EDITOR