#pragma once
#include <Tridium/Core/Enum.h>
#include <Tridium/Core/Types.h>
#include <Tridium/Utils/Macro.h>

namespace Tridium {
    namespace Refl {

        enum class EClassFlags : uint8_t
        {
            ECF_None = 0,
            Scriptable = BIT( 0 ),
        };

        enum class EPropertyFlags : uint8_t
        {
            EPF_None = 0,
            Serialize       = BIT( 0 ), /* Defines this property as serializable. */
            EditAnywhere    = BIT( 1 ), /* Defines this property as Editable from the Editor Inspector. */
            VisibleAnywhere = BIT( 2 ), /* Defines this property as Visible from the Editor Inspector. */
            ScriptReadOnly  = BIT( 3 ), /* Defines this property as Read-Only from a Script. */
            ScriptReadWrite = BIT( 4 ), /* Defines this property as Read-Write from a Script. */
        };

    } // namespace Refl
} // namespace Tridium

DEFINE_BITMASK_OPS( Tridium::Refl::EClassFlags );
DEFINE_BITMASK_OPS( Tridium::Refl::EPropertyFlags );