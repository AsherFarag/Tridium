#pragma once
#ifdef IS_EDITOR
#include <Tridium/Core/Delegate.h>
#include <Tridium/Core/Types.h>

// Forward Declarations
namespace Tridium {
}
// --------------------

namespace Tridium::Editor {

	class Events 
	{
	public:
		static MulticastDelegate<void, GameObjectID> OnGameObjectSelected;
	};

}

#endif // IS_EDITOR