#include "tripch.h"
#include "Scene.h"
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Component.h>
#include <Tridium/ECS/Components/Types.h>

// Temp ?
#include <Tridium/Rendering/Renderer.h>
#include <Tridium/Rendering/RenderCommand.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/ECS/Components/Types/Rendering/MeshComponent.h>

namespace Tridium {

	Scene::Scene()
	{

	}

	Scene::~Scene()
	{
	}

	void Scene::Update()
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

			for ( auto entity : storage.second )
				static_cast<ScriptableComponent*>( storage.second.value( entity ) )->OnUpdate();
		}

		// Render all Scene Cameras
		auto& cameras = m_Registry.view<CameraComponent, TransformComponent>();
		cameras.each( [&]( auto entity, CameraComponent& camera, TransformComponent& transform )
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
		meshComponents.each( [&]( auto entity, MeshComponent& mesh, TransformComponent& transform )
			{
				mesh.GetShader()->Bind();
				Vector4 colour = Vector4( (float)( glm::sin( Time::Get() + 10.f ) * 0.5f + 0.5f ),
					(float)( glm::sin( Time::Get() ) * 0.5f + 0.5f ),
					(float)( glm::sin( Time::Get() - 10.f ) * 0.5f + 0.5f ), 1.0 );


				mesh.GetShader()->SetFloat4( "uColour", colour );
				Renderer::Submit( mesh.GetShader(), mesh.GetMesh().VAO, transform.GetTransform() );
			} );

		Renderer::EndScene();
	}

	GameObject Scene::InstantiateGameObject( const std::string& a_Name )
	{
		auto go = GameObject( m_Registry.create() );
		go.Init( a_Name );
		return go;
	}

}