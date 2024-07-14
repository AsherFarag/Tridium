#include "tripch.h"
#include "Mesh.h"
#include <Tridium/Core/Application.h>

#include <Tridium/Rendering/Buffer.h>
#include <Tridium/Rendering/VertexArray.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Tridium {


    MeshImportSettings::MeshImportSettings()
    {
        PostProcessFlags = aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GlobalScale;
    }

#pragma region MeshImporter

    Ref<Mesh> MeshImporter::Import( const std::string& filepath, const MeshImportSettings& importSettings )
    {
        Assimp::Importer importer;
        importer.SetPropertyFloat( AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, importSettings.Scale );

        const aiScene* scene = importer.ReadFile( filepath, importSettings.PostProcessFlags );
        if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
        {
            TE_CORE_ERROR( "ASSIMP: File - '{0}', Error - {1}", filepath, importer.GetErrorString() );
            return nullptr;
        }

        // Create a new Mesh
        Ref<Mesh> loadedMesh = MakeRef<Mesh>();

        TODO( "We currently only load the first mesh for simplicity!" );
        aiMesh* ai_mesh = scene->mMeshes[0];

        #pragma region Load Verticies

        loadedMesh->m_NumVerticies = ai_mesh->mNumVertices;
        loadedMesh->m_Verticies.resize( ai_mesh->mNumVertices );
        if ( ai_mesh->HasPositions() )
        {
            for ( uint32_t i = 0; i < ai_mesh->mNumVertices; ++i )
            {
                loadedMesh->m_Verticies[i].Position = Vector3( ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z );
            }
        }

        if ( ai_mesh->HasNormals() )
        {
            for ( uint32_t i = 0; i < ai_mesh->mNumVertices; ++i )
                loadedMesh->m_Verticies[i].Normal = Vector3( ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z );
        }

        for ( uint32_t channel = 0; channel < Vertex::NumUVChannels; ++channel )
        {
            if ( ai_mesh->HasTextureCoords( channel ) )
            {
                for ( uint32_t i = 0; i < ai_mesh->mNumVertices; ++i )
                    loadedMesh->m_Verticies[i].UV[channel] = Vector2(ai_mesh->mTextureCoords[channel][i].x, ai_mesh->mTextureCoords[channel][i].y);
            }
        }

        if ( ai_mesh->HasTangentsAndBitangents() )
        {
            for ( uint32_t i = 0; i < ai_mesh->mNumVertices; ++i )
                loadedMesh->m_Verticies[i].Tangent = Vector3( ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z );
        }

        #pragma endregion

        // If there were no verticies, return
        if ( loadedMesh->m_Verticies.empty() )
        {
            return false;
        }

        loadedMesh->m_NumVerticies = loadedMesh->m_Verticies.size();

        // Load indices
        std::vector<uint32_t> indices;
        indices.reserve( ai_mesh->mNumFaces * 3 );
        for ( unsigned int i = 0; i < ai_mesh->mNumFaces; ++i ) 
        {
            aiFace face = ai_mesh->mFaces[i];
            for ( unsigned int j = 0; j < face.mNumIndices; ++j )
            {
                indices.push_back( face.mIndices[j] );
            }
        }

        // If there were no indices, return
        if ( indices.empty() )
        {
            return false;
        }

        loadedMesh->m_NumIndicies = indices.size();

        // - Load the mesh objects into the GPU -

        // Create Vertex Objects
        loadedMesh->m_VAO = VertexArray::Create();
        loadedMesh->m_VBO = VertexBuffer::Create( (float*)loadedMesh->m_Verticies.data(), loadedMesh->m_Verticies.size() * sizeof( Vertex ) );

        BufferLayout layout =
        {
            { ShaderDataType::Float3, "aPosition" },
            { ShaderDataType::Float3, "aNormal" },
            { ShaderDataType::Float2, "aUV", Vertex::NumUVChannels },
            { ShaderDataType::Float3, "aTangent" }
        };

        loadedMesh->m_VBO->SetLayout( layout );
        loadedMesh->m_VAO->AddVertexBuffer( loadedMesh->m_VBO );

        loadedMesh->m_IBO = IndexBuffer::Create( indices.data(), indices.size() );
        loadedMesh->m_VAO->SetIndexBuffer( loadedMesh->m_IBO );

        loadedMesh->m_Path = filepath;

        if ( importSettings.DiscardLocalData )
        {
            loadedMesh->m_Verticies.clear();
        }

        return loadedMesh;
    }

#pragma endregion

    Ref<Mesh> Mesh::Load( const std::string& path )
    {
        return MeshImporter::Import(path);
    }

    const Ref<Mesh>& Mesh::GetQuad()
    {
        static Ref<Mesh> quadMesh;
        if ( !quadMesh )
        {
            quadMesh = MakeRef<Mesh>();

            quadMesh->m_VAO = VertexArray::Create();

            float vertices[] = {
                -0.5f, -0.5f,  0, 0.0f, 0.0f,
                -0.5f,  0.5f,  0, 0.0f, 1.0f,
                 0.5f, -0.5f,  0, 1.0f, 0.0f,
                 0.5f,  0.5f,  0, 1.0f, 1.0f
            };

            uint32_t indices[] = { 0,1,2,1,2,3 };

            quadMesh->m_VBO = VertexBuffer::Create( vertices, sizeof( vertices ) );

            BufferLayout layout =
            {
                { ShaderDataType::Float3, "aPosition" },
                { ShaderDataType::Float2, "aUV" },
            };

            quadMesh->m_VBO->SetLayout( layout );
            quadMesh->m_VAO->AddVertexBuffer( quadMesh->m_VBO );

            quadMesh->m_IBO = IndexBuffer::Create( indices, sizeof( indices ) / sizeof( uint32_t ) );
            quadMesh->m_VAO->SetIndexBuffer( quadMesh->m_IBO );

            TODO( "Make a better quad mesh system!" );
            quadMesh->m_GUID = 1;
        }

        return quadMesh;
    }
}