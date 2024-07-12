#include "tripch.h"
#include "Asset.h"

#ifdef IS_EDITOR
#include <Editor/Editor.h>
#endif // IS_EDITOR



namespace Tridium {

    template<typename T>
    Ref<T> Asset<T>::Create()
    {
        Ref<T> asset = MakeRef<T>();
        asset->m_MetaData.ID = GUID::Create();
        return ;
    }

}