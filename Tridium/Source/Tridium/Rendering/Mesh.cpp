#include "tripch.h"
#include "Mesh.h"
#include <Tridium/Core/Application.h>

#include <Tridium/Rendering/Buffer.h>
#include <Tridium/Rendering/VertexArray.h>

namespace Tridium {

    const AssetRef<Mesh>& Mesh::GetQuad()
    {
        static Mesh quadMesh;
        static bool init = false;
        if ( !init )
        {
            quadMesh.Internal_AddRef();
            quadMesh.m_VAO = VertexArray::Create();

            float vertices[] = {
                -0.5f, -0.5f,  0, 0.0f, 0.0f,
                -0.5f,  0.5f,  0, 0.0f, 1.0f,
                 0.5f, -0.5f,  0, 1.0f, 0.0f,
                 0.5f,  0.5f,  0, 1.0f, 1.0f
            };

            uint32_t indices[] = { 0,1,2,1,2,3 };

            quadMesh.m_VBO = VertexBuffer::Create( vertices, sizeof( vertices ) );

            BufferLayout layout =
            {
                { ShaderDataType::Float3, "aPosition" },
                { ShaderDataType::Float2, "aUV" },
            };

            quadMesh.m_VBO->SetLayout( layout );
            quadMesh.m_VAO->AddVertexBuffer( quadMesh.m_VBO );

            quadMesh.m_IBO = IndexBuffer::Create( indices, sizeof( indices ) / sizeof( uint32_t ) );
            quadMesh.m_VAO->SetIndexBuffer( quadMesh.m_IBO );

            TODO( "Make a better quad mesh system!" );
            quadMesh.m_Handle = 1;
        }

        return { &quadMesh };
    }
}