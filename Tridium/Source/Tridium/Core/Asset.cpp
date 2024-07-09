#include "tripch.h"
#include "Asset.h"

#ifdef IS_EDITOR
#include <Editor/Editor.h>
#endif // IS_EDITOR



namespace Tridium {

	void Asset::SetModified( bool modified )
	{
        if ( m_Modified == modified )
            return;

        m_Modified = modified;
	}

}