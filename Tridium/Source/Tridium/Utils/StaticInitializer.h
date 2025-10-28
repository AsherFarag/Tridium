#pragma once
#include <Tridium/Common/Function.h>
#include <Tridium/Utils/Macro.h>

namespace Tridium {

#define DECLARE_INITIALIZER(_Name) \
    namespace StaticInitializers { \
        struct CONCAT( Initializer_, EXPAND( _Name ) ) { \
            template<typename _Func> \
            CONCAT( Initializer_, EXPAND( _Name ) )(_Func&& a_Func) { a_Func(); } \
        }; \
    }

#define DEFINE_INITIALIZER(_Name) \
    static volatile StaticInitializers::CONCAT( Initializer_, EXPAND( _Name ) ) CONCAT( __StaticInitializer_, EXPAND( _Name ) ) = +[]()

} // namespace Tridium