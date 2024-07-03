#include "tripch.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Tridium {

#pragma region MeshLoader

    bool MeshLoader::Load( const std::string& filepath, MeshHandle& outMeshHandle )
    {
        if ( MeshLibrary::GetHandle( filepath, outMeshHandle ) )
            return true;

        Assimp::Importer importer;

        unsigned int aiPostProcessFlags =// aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals;
        aiProcess_GlobalScale
            | aiProcess_OptimizeMeshes
            | aiProcess_OptimizeGraph
            | aiProcess_RemoveRedundantMaterials
            | aiProcess_Triangulate
            | aiProcess_LimitBoneWeights
            | aiProcess_SplitByBoneCount
            | aiProcess_CalcTangentSpace;
        const aiScene* scene = importer.ReadFile( filepath, aiPostProcessFlags );

        if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
        {
            TE_CORE_ERROR( "ASSIMP: {0}", importer.GetErrorString() );
            return false;
        }

        // Create a new Mesh
        Ref<Mesh> loadedMesh = MakeRef<Mesh>();

        TODO( "We currently only load the first mesh for simplicity!" );
        aiMesh* ai_mesh = scene->mMeshes[0];

#pragma region Load Verticies

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

        TODO( "We only use the first channel of TexCoords from Assimp!" );
        if ( ai_mesh->HasTextureCoords(0) )
        {
            for ( uint32_t i = 0; i < ai_mesh->mNumVertices; ++i )
                loadedMesh->m_Verticies[i].UV = Vector2( ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y );
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

        // Load indices
        std::vector<uint32_t> indices( ai_mesh->mNumFaces );
        for ( unsigned int i = 0; i < ai_mesh->mNumFaces; ++i ) {
            aiFace face = ai_mesh->mFaces[i];
            for ( unsigned int j = 0; j < face.mNumIndices; ++j ) {
                indices.push_back( face.mIndices[j] );
            }
        }

        // If there were no indices, return
        if ( indices.empty() )
        {
            return false;
        }

        // - Load the mesh objects into the GPU -

        // Create Vertex Objects
        loadedMesh->m_VAO = VertexArray::Create();
        loadedMesh->m_VBO = VertexBuffer::Create( (float*)loadedMesh->m_Verticies.data(), loadedMesh->m_Verticies.size() * sizeof( Vertex ));

        BufferLayout layout =
        {
            { ShaderDataType::Float3, "aPosition" },
            { ShaderDataType::Float3, "aNormal" },
            { ShaderDataType::Float2, "aUV" },
            { ShaderDataType::Float3, "aTangent" }
        };

        loadedMesh->m_VBO->SetLayout( layout );
        loadedMesh->m_VAO->AddVertexBuffer( loadedMesh->m_VBO );

        loadedMesh->m_IBO = IndexBuffer::Create( indices.data(), indices.size() );
        loadedMesh->m_VAO->SetIndexBuffer( loadedMesh->m_IBO );

        loadedMesh->m_FilePath = filepath;
        MeshLibrary::AddMesh( filepath, loadedMesh, outMeshHandle );
        return true;
    }

#pragma endregion


#pragma region MeshLibrary

    MeshLibrary* MeshLibrary::Get()
    {
        static bool hasBeenInit = false;
        static MeshLibrary* s_Instance = new MeshLibrary();

        if (!hasBeenInit)
        {
            hasBeenInit = true;
            s_Instance->InitPrimatives();
        }

        return s_Instance;
    }

    Ref<Mesh> MeshLibrary::GetMesh( const MeshHandle& meshHandle )
    {
        if ( auto pair = Get()->m_LoadedMeshes.find( meshHandle ); pair != Get()->m_LoadedMeshes.end() )
            return pair->second;

        return nullptr;
    }

    bool MeshLibrary::GetHandle( const std::string& filePath, MeshHandle& outMeshHandle )
    {
        auto& pathHandles = Get()->m_PathHandles;
        if ( auto pair = pathHandles.find( filePath ); pair != pathHandles.end() )
        {
            outMeshHandle = pair->second;
            return true;
        }
        return false;
    }

    void MeshLibrary::InitPrimatives()
    {
#pragma region Quad

        Ref<Mesh> quadMesh = MakeRef<Mesh>();

        quadMesh->m_VAO = VertexArray::Create();

        float vertices[] = {
            -0.5f, -0.5f,  0,	0.0f, 0.0f, 1.0f,	 0.0f, 0.0f,
            -0.5f,  0.5f,  0,	0.0f, 0.0f, 1.0f,	 0.0f, 1.0f,
             0.5f, -0.5f,  0,	0.0f, 0.0f, 1.0f,	 1.0f, 0.0f,
             0.5f,  0.5f,  0,	0.0f, 0.0f, 1.0f,	 1.0f, 1.0f
        };

        uint32_t indices[] = { 0,1,2,1,2,3 };

        quadMesh->m_VBO = VertexBuffer::Create( vertices, sizeof( vertices ) );

        BufferLayout layout =
        {
            { ShaderDataType::Float3, "aPosition" },
            { ShaderDataType::Float3, "aNormal" },
            { ShaderDataType::Float2, "aUV" },
        };

        quadMesh->m_VBO->SetLayout( layout );
        quadMesh->m_VAO->AddVertexBuffer( quadMesh->m_VBO );

        quadMesh->m_IBO = IndexBuffer::Create( indices, sizeof( indices ) / sizeof( uint32_t ) );
        quadMesh->m_VAO->SetIndexBuffer( quadMesh->m_IBO );

        AddMesh( "Quad", quadMesh, m_Quad );

#pragma endregion
    }

    void MeshLibrary::AddMesh( const std::string& filePath, const Ref<Mesh>& mesh, MeshHandle& outMeshHandle )
    {
        auto& pathHandles = Get()->m_PathHandles;
        if ( auto pair = pathHandles.find( filePath ); pair != pathHandles.end() )
        {
            outMeshHandle = pair->second;
            return;
        }

        outMeshHandle = CreateGUID();
        pathHandles.emplace( filePath, outMeshHandle );

        Get()->m_LoadedMeshes[outMeshHandle] = mesh;
    }

#pragma endregion

}