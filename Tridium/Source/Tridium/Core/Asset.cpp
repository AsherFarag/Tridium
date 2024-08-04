#include "tripch.h"
#include "Asset.h"
#include <Tridium/Core/Application.h>

namespace Tridium {

    void Asset::Release()
    {
        if ( --m_RefCount == 0 )
        {
            // Clean up the asset if reference count drops to zero
            delete this;
        }
    }
}