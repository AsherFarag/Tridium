#include "tripch.h"
#include "D3D12Mesh.h"

namespace Tridium {

	//////////////////////////////////////////////////////////////////////////
	// D3D12 Index Buffer
	//////////////////////////////////////////////////////////////////////////

    bool D3D12IndexBuffer::Commit( const void* a_Params )
    {
        return false;
    }

    bool D3D12IndexBuffer::Release()
    {
        return false;
    }

    bool D3D12IndexBuffer::IsValid() const
    {
        return false;
    }

    const void* D3D12IndexBuffer::NativePtr() const
    {
        return nullptr;
    }



	//////////////////////////////////////////////////////////////////////////
	// D3D12 Vertex Buffer
	//////////////////////////////////////////////////////////////////////////

	bool D3D12VertexBuffer::Commit( const void* a_Params )
	{
		return false;
	}

	bool D3D12VertexBuffer::Release()
	{
		return false;
	}

	bool D3D12VertexBuffer::IsValid() const
	{
		return false;
	}

	const void* D3D12VertexBuffer::NativePtr() const
	{
		return nullptr;
	}

}
