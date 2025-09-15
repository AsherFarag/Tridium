#include "tripch.h"
#include "MeshAsset.h"
#include "AssetDatabase.h"

namespace Tridium {

	void StaticMesh::UpdateBoundingBox()
	{
		m_BoundingBox = {};
		for ( const auto& lod : m_LODs )
		{
			for ( const auto& vertex : lod.Vertices )
			{
				m_BoundingBox.Expand( vertex.Position );
			}
		}
	}

}
