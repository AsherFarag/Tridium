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

		TODO( "Make this only happen if the camera is shown!" );
		// Render all Scene Cameras
		auto& cameras = m_Registry.view<CameraComponent, TransformComponent>();
		cameras.each( [&]( auto go, CameraComponent& camera, TransformComponent& transform )
			{
				auto orientation = Quaternion( Vector3( -transform.Rotation.x, -transform.Rotation.y, 0.f ) );
				Matrix4 viewMatrix = glm::translate( Matrix4( 1.f ), transform.Position ) * glm::toMat4( orientation );
				viewMatrix = glm::inverse( viewMatrix );

				Render( camera.SceneCamera, viewMatrix );
			});
	}

	void Scene::Render( const Camera& camera, const Matrix4& viewMatrix )
	{
		RenderCommand::SetClearColor( { 0.1, 0.1, 0.12, 1.0 } );
		RenderCommand::Clear();

		Renderer::BeginScene( camera, viewMatrix );

		auto meshComponents = m_Registry.view<MeshComponent, TransformComponent>();

		Ref<Shader> currentShader = nullptr;

		meshComponents.each( [&]( auto go, MeshComponent& mesh, TransformComponent& transform )
			{
				if ( mesh.GetShader() != currentShader )
				{
					currentShader = mesh.GetShader();
					currentShader->Bind();
				}

				Vector4 colour = Vector4( (float)( glm::sin( Time::Get() + 10.f ) * 0.5f + 0.5f ),
					(float)( glm::sin( Time::Get() ) * 0.5f + 0.5f ),
					(float)( glm::sin( Time::Get() - 10.f ) * 0.5f + 0.5f ), 1.0 );


				currentShader->SetFloat4( "uColour", { 0.2,0.3,1,1 } );
				Renderer::Submit( mesh.GetShader(), mesh.GetMesh().VAO, transform.GetWorldTransform() );
			} );


		auto spriteComponents = m_Registry.view<SpriteComponent, TransformComponent>();

		spriteComponents.each( [ & ]( auto go, SpriteComponent& sprite, TransformComponent& transform )
			{
				if ( sprite.GetShader() != currentShader )
				{
					currentShader = sprite.GetShader();
					currentShader->Bind();
				}

				Vector3 oldScale = transform.Scale;

				if ( sprite.GetTexture() )
				{
					sprite.GetTexture()->Bind();

					Vector2 textureSize( sprite.GetTexture()->GetWidth(), sprite.GetTexture()->GetHeight() );
					if ( textureSize.x > textureSize.y )
					{
						transform.Scale.y *= textureSize.y / textureSize.x;
					}
					else
					{
						transform.Scale.x *= textureSize.x / textureSize.y;
					}
				}

				Renderer::Submit( sprite.GetShader(), sprite.GetMesh().VAO, transform.GetWorldTransform() );
				transform.Scale = oldScale;

				if ( sprite.GetTexture() )
				{
					sprite.GetTexture()->Unbind();
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