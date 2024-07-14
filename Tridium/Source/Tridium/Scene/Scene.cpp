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
#include <Tridium/Rendering/Material.h>
#include <Tridium/Rendering/Mesh.h>

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

		Matrix4 pvm = camera.GetProjection() * viewMatrix;
		Renderer::BeginScene( camera, viewMatrix );

		// - Draw Meshes -
		RenderCommand::SetCullMode( true );
		auto meshComponents = m_Registry.view<MeshComponent, TransformComponent>();
		meshComponents.each( [&]( auto go, MeshComponent& meshComponent, TransformComponent& transform )
			{
				if ( auto& mesh = meshComponent.GetMesh() )
				{
					Ref<Material> mat = meshComponent.GetMaterial();
					Ref<Shader> shader = mat ? mat->GetShader() : nullptr;

					if ( !shader )
						return;

					shader->Bind();
					shader->SetMatrix4( "uPVM", pvm * transform.GetWorldTransform() );

					if ( mat )
					{
						shader->SetFloat4( "uColor", mat->Color );
						mat->Bind();
					}

					mesh->GetVAO()->Bind();

					RenderCommand::DrawIndexed( mesh->GetVAO() );

					mesh->GetVAO()->Unbind();

					if ( mat )
						mat->Unbind();

					shader->Unbind();
				}
			} );


		// - Draw Sprites -
		RenderCommand::SetCullMode( false );

		//auto spriteShader = ShaderLibrary::GetShader(  ShaderLibrary::GetSpriteShader() );
		//auto quadMeshVAO = MeshLibrary::GetMesh( MeshLibrary::GetQuad() )->GetVAO();

		//spriteShader->Bind();
		//quadMeshVAO->Bind();

		//auto spriteComponents = m_Registry.view<SpriteComponent, TransformComponent>();
		//spriteComponents.each( 
		//	[ & ]( auto go, SpriteComponent& sprite, TransformComponent& transform )
		//	{
		//		spriteShader->SetMatrix4( "uPVM", pvm * transform.GetWorldTransform() );

		//		Ref<Texture> tex = TextureLibrary::GetTexture( sprite.GetTexture() );
		//		if ( tex )
		//			tex->Bind();

		//		RenderCommand::DrawIndexed( quadMeshVAO );

		//		if ( tex )
		//			tex->Unbind();
		//	} );

		//spriteShader->Unbind();
		//quadMeshVAO->Unbind();

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