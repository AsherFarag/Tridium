#pragma once
#ifdef IS_EDITOR
#include <Tridium/Core/Delegate.h>
#include <Tridium/Core/Types.h>

// Forward Declarations
namespace Tridium {
	class GameObject;
}
// --------------------

namespace Tridium {

	class Events 
	{
	public:
		static MulticastDelegate<void, GameObject> OnGameObjectSelected;
	};

}

#endif // IS_EDITOR