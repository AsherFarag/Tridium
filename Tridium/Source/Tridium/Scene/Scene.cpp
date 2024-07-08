#include "tripch.h"
#include "Scene.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/ECS/Components/Types.h>

// Temp ?
#include <Tridium/Rendering/Renderer.h>
#include <Tridium/Rendering/RenderCommand.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/ECS/Components/Types/Rendering/MeshComponent.h>

namespace Tridium {

	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::OnBegin()
	{
		// Can call 'OnBegin' on all game objects or something
	}

	void Scene::OnUpdate()
	{
		// Update scriptable objects
		for ( const auto& storage : m_Registry.storage() )
		{
			// Nothing to iterate through, Continue
			if ( storage.second.size() == 0 )
				continue;

			// We know that the data will always be inherited from Component
			void* data = storage.second.value( storage.second[ 0 ] );
			Component* component = (Component*)data;

			// If the type is not inherited from ScriptableComponent, continue 
			if ( dynamic_cast<ScriptableComponent*>( component ) == nullptr )
				continue;

			for ( auto go : storage.second )
				reinterpret_cast<ScriptableComponent*>( storage.second.value( go ) )->OnUpdate();
		}
	}

	void Scene::Render( const Camera& camera, const Matrix4& viewMatrix )
	{
		RenderCommand::SetClearColor( { 0.1, 0.1, 0.12, 1.0 } );
		RenderCommand::Clear();

		Renderer::BeginScene( camera, viewMatrix );

		auto meshComponents = m_Registry.view<MeshComponent, TransformComponent>();

		MaterialHandle currentMaterialHandle;
		Ref<Material> currentMaterial = nullptr;
		MeshHandle currentMeshHandle = {};
		Ref<Mesh> currentMesh = nullptr;


		RenderCommand::SetCullMode( true );
		meshComponents.each( [&]( auto go, MeshComponent& mesh, TransformComponent& transform )
			{
				if ( mesh.GetMesh() != currentMeshHandle )
				{
					currentMeshHandle = mesh.GetMesh();
					currentMesh = MeshLibrary::GetMesh( currentMeshHandle );
				}

				if ( currentMesh )
				{
					if ( mesh.GetMaterial() != currentMaterialHandle )
					{
						currentMaterialHandle = mesh.GetMaterial();
						currentMaterial = MaterialLibrary::GetMaterial( currentMaterialHandle );
					}

					Renderer::Submit( currentMaterial, currentMesh->GetVAO(), transform.GetWorldTransform());
				}
			} );

		RenderCommand::SetCullMode( false );
		auto quadMeshVAO = MeshLibrary::GetMesh( MeshLibrary::GetQuad() )->GetVAO();
		TextureHandle currentTexHandle;
		Ref<Texture> currentTex = nullptr;
		auto spriteComponents = m_Registry.view<SpriteComponent, TransformComponent>();
		spriteComponents.each( [ & ]( auto go, SpriteComponent& sprite, TransformComponent& transform )
			{
				if ( currentTexHandle != sprite.GetTexture() )
				{
					currentTexHandle = sprite.GetTexture();
					currentTex = TextureLibrary::GetTexture( currentTexHandle );
				}

				Vector3 oldScale = transform.Scale;

				if ( currentTex )
				{
					currentTex->Bind();

					Vector2 textureSize( currentTex->GetWidth(), currentTex->GetHeight() );
					if ( textureSize.x > textureSize.y )
					{
						transform.Scale.y *= textureSize.y / textureSize.x;
					}
					else
					{
						transform.Scale.x *= textureSize.x / textureSize.y;
					}
				}

				Renderer::Submit( ShaderLibrary::GetShader( sprite.GetShader() ), quadMeshVAO, transform.GetWorldTransform());
				transform.Scale = oldScale;

				if ( currentTex )
				{
					currentTex->Unbind();
				}
			} );

		Renderer::EndScene();
	}

	void Scene::OnEnd()
	{
	}

	GameObject Scene::InstantiateGameObject( const std::string& name )
	{
		auto go = GameObject( m_Registry.create() );
		go.AddComponent<GUIDComponent>();
		go.AddComponent<TagComponent>( name );
		go.AddComponent<TransformComponent>();
		return go;
	}

	GameObject Scene::InstantiateGameObject( GUID guid, const std::string& name )
	{
		auto go = GameObject( m_Registry.create() );
		go.AddComponent<GUIDComponent>( guid );
		go.AddComponent<TagComponent>( name );
		go.AddComponent<TransformComponent>();
		return go;
	}

	void Scene::Clear()
	{
		m_Registry.clear();
	}

	CameraComponent* Scene::GetMainCamera()
	{
		// If the scene doesn't have a main camera,
		// get the first camera in the scene
		if ( !m_Registry.valid( m_MainCamera ) )
		{
			auto cameras = m_Registry.view<CameraComponent>();
			// Return nullptr as there are no cameras in the scene
			if ( cameras.empty() )
				return nullptr;

			m_MainCamera = cameras.front();
		}
		return m_Registry.try_get<CameraComponent>( m_MainCamera );
	}

}