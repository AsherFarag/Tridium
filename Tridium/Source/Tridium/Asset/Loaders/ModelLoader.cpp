#include "tripch.h"
#include "ModelLoader.h"
#include <Tridium/Rendering/Mesh.h>
#include <Tridium/Asset/Meta/ModelMetaData.h>

#include <Tridium/Rendering/Buffer.h>
#include <Tridium/Rendering/VertexArray.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Tridium {

	Mesh* ModelLoader::Load( const IO::FilePath& a_Path, const ModelMetaData& a_MetaData )
	{
        Assimp::Importer importer;
        importer.SetPropertyFloat( AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, a_MetaData.ImportSettings.Scale );

        const aiScene* scene = importer.ReadFile( a_Path.ToString(), a_MetaData.ImportSettings.PostProcessFlags);
        if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode )
        {
            TE_CORE_ERROR( "ASSIMP: File - '{0}', Error - {1}", a_Path.ToString(), importer.GetErrorString());
            return nullptr;
        }

        // Create a new Mesh
        Mesh* loadedMesh = new Mesh{};

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
                    loadedMesh->m_Verticies[i].UV[channel] = Vector2( ai_mesh->mTextureCoords[channel][i].x, ai_mesh->mTextureCoords[channel][i].y );
            }
        }

        if ( ai_mesh->HasTangentsAndBitangents() )
        {
            for ( uint32_t i = 0; i < ai_mesh->mNumVertices; ++i )
                loadedMesh->m_Verticies[i].Tangent = Vector3( ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z );
        }

        // If there were no verticies, return
        if ( loadedMesh->m_Verticies.empty() )
        {
            delete loadedMesh;
            return nullptr;
        }

        loadedMesh->m_NumVerticies = loadedMesh->m_Verticies.size();

        #pragma endregion

        #pragma region Load Indices

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
            delete loadedMesh;
            return nullptr;
        }

        loadedMesh->m_NumIndicies = indices.size();

        #pragma endregion

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

        if ( a_MetaData.ImportSettings.DiscardLocalData )
        {
            loadedMesh->m_Verticies.clear();
        }

        // Set meta data
        loadedMesh->m_Handle = a_MetaData.Handle;
        loadedMesh->m_Loaded = true;
        loadedMesh->m_Path = a_Path.ToString();

        return loadedMesh;
	}

}