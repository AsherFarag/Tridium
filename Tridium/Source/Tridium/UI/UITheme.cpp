#include "UITheme.h"

namespace Tridium::UI {

    static UITheme s_Theme{};

    UITheme& GetTheme()
    {
        return s_Theme;
    }

} // namespace Tridium::UI
