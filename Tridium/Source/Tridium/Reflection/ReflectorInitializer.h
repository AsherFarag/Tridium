//
// NOTE:
// A frustrating issue with the reflection system is that it requires the files it is declared in to be linked by the linker.
// However, the linker will not link the files if they are not used in the code. (Stupid MSVC)
// To get around this, you can include your reflected class here via calling MyClass::__Tridium_Reflect(); ( which is added via the REFLECT(Class) macro ).
// This will force the linker to link the file and allow the reflection system to work.

#pragma once
#include <Tridium/ECS/Components/Types/Common/CharacterControllerComponent.h>

namespace Tridium::Refl::Internal {

	class HiddenTypeReflector
	{
	public:
		static void ReflectHiddenTypes()
		{
			// Include your reflected classes here
			// Example:
			// MyClass::__Tridium_Reflect();

			CharacterControllerComponent::__Tridium_Reflect();
		}
	};

} // namespace Tridium::Refl::Internal