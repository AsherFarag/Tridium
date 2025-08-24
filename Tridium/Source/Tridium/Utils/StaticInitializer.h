#pragma once
#include <Tridium/Common/Function.h>
#include <Tridium/Utils/Macro.h>

namespace Tridium {

#define DECLARE_INITIALIZER(_Name) \
    namespace StaticInitializers { \
        struct Initializer_##_Name { \
            template<typename _Func> \
            Initializer_##_Name(_Func&& a_Func) { a_Func(); } \
        }; \
    }

#define DEFINE_INITIALIZER(_Name) \
    static volatile StaticInitializers::Initializer_##_Name __StaticInitializer_##_Name = +[]()


} // namespace Tridium