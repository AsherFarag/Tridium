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

	void Scene::Render( const Camera& a_Camera, const Matrix4& a_ViewMatrix )
	{
		RenderCommand::SetClearColor( { 0.1, 0.1, 0.12, 1.0 } );
		RenderCommand::Clear();

		Matrix4 pvm = a_Camera.GetProjection() * a_ViewMatrix;
		Renderer::BeginScene( a_Camera, a_ViewMatrix );

		// - Draw Meshes -
		RenderCommand::SetCullMode( true );
		auto meshComponents = m_Registry.view<MeshComponent, TransformComponent>();
		meshComponents.each( [&]( auto go, MeshComponent& meshComponent, TransformComponent& transform )
			{
				//if ( true /*auto& mesh = meshComponent.GetMesh()*/ )
				//{
				//	AssetRef<Material> mat = meshComponent.GetMaterial();
				//	AssetRef<Shader> shader = mat ? mat->GetShader() : nullptr;

				//	if ( !shader )
				//		return;

				//	shader->Bind();
				//	shader->SetMatrix4( "uPVM", pvm * transform.GetWorldTransform() );

				//	if ( mat )
				//	{
				//		//shader->SetFloat4( "uColor", mat->Color );
				//		mat->Bind();
				//	}

				//	mesh->GetVAO()->Bind();

				//	RenderCommand::DrawIndexed( mesh->GetVAO() );

				//	mesh->GetVAO()->Unbind();

				//	if ( mat )
				//		mat->Unbind();

				//	shader->Unbind();
				//}

				std::string vert = R"(
					#version 330 core
					layout(location = 0) in vec3 a_Position;
					layout(location = 1) in vec3 a_Normal; 

					uniform mat4 uPVM;
					uniform mat4 uModel;
					
					out vec3 v_Normal;

					void main()
					{
						v_Normal = mat3(transpose(inverse(uModel))) * a_Normal;
						gl_Position = uPVM * vec4(a_Position, 1.0);
					}
				)";

				std::string frag = R"(
				    #version 330 core
				    layout(location = 0) out vec4 color;
				
				    in vec3 v_Normal;
				
				    uniform vec3 lightDir; // Direction of the light
				    uniform vec3 objectColor; // Base object color
					uniform vec3 ambientColor; // Ambient color
					uniform vec3 lightColor; // Light color
				
				    void main()
				    {
				        // Normalize the normal vector
				        vec3 norm = normalize(v_Normal);
				
				        // Calculate diffuse lighting (Lambertian reflectance)
				        float diff = max(dot(norm, normalize(lightDir)), 0.0);
				
				        // Final color is object color modulated by diffuse lighting
				        vec3 lighting = objectColor * diff;
				
				        // Output final color with alpha of 1.0
				        color = vec4(ambientColor + lighting + lightColor * diff, 1.0f);
				    }
				)";
				RenderCommand::SetCullMode( false );
				static Shader* shader = Shader::Create( vert, frag );

				shader->Bind();
				shader->SetMatrix4( "uPVM", pvm * transform.GetWorldTransform() );
				shader->SetMatrix4( "uModel", transform.GetWorldTransform() );
				shader->SetFloat3( "lightDir", { 1.0f, 1.0f, -1.0f } );
				shader->SetFloat3( "objectColor", { 0.5f, 0.5f, 0.5f } );
				shader->SetFloat3( "ambientColor", { 0.3f, 0.1f, 0.05f } );
				shader->SetFloat3( "lightColor", { 1.0f, 0.95f, 0.1f } );

				auto& meshRef = meshComponent.GetMesh();
				auto& mesh = *meshRef.Get();

				mesh.GetVAO()->Bind();

				RenderCommand::DrawIndexed( mesh.GetVAO() );

				mesh.GetVAO()->Unbind();

				shader->Unbind();
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

		//		SharedPtr<Texture> tex = TextureLibrary::GetTexture( sprite.GetTexture() );
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

	GameObject Scene::InstantiateGameObject( const std::string& a_Name )
	{
		auto go = GameObject( m_Registry.create() );
		go.AddComponent<GUIDComponent>();
		go.AddComponent<TagComponent>( a_Name );
		go.AddComponent<TransformComponent>();
		return go;
	}

	GameObject Scene::InstantiateGameObject( GUID a_GUID, const std::string& a_Name )
	{
		auto go = GameObject( m_Registry.create() );
		go.AddComponent<GUIDComponent>( a_GUID );
		go.AddComponent<TagComponent>( a_Name );
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