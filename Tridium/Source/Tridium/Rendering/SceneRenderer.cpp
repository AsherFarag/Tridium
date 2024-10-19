#include "tripch.h"
#include "SceneRenderer.h"
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Rendering/Renderer.h>
#include <Tridium/Rendering/RenderCommand.h>
#include <Tridium/Rendering/Shader.h>
#include <Tridium/Rendering/Texture.h>
#include <Tridium/Rendering/Material.h>
#include <Tridium/Rendering/Mesh.h>

namespace Tridium {

	Vector3 s_AmbientColor = { 0.1f, 0.1f, 0.1f };
	Vector3 s_LightDirection = { 0.0f, 1.0f, 0.0f };
	Vector3 s_LightColor = { 1.0f, 0.5f, 0.0f };
	float s_LightIntensity = 1.0f;

	SceneRenderer::SceneRenderer( const SharedPtr<Scene>& a_Scene )
		: m_Scene( a_Scene )
	{
		const std::string vert = R"(
					#version 410 core
					layout(location = 0) in vec3 a_Position;
					layout(location = 1) in vec3 a_Normal;
					layout(location = 2) in vec3 a_Tangent;
					layout(location = 3) in vec3 a_Bitangent;
					layout(location = 4) in vec2 a_UV;


					uniform mat4 u_PVM;
					uniform mat4 u_Model;
					
					out vec3 v_Normal;
					out vec3 v_Tangent;
					out vec3 v_Bitangent;
					out vec2 v_UV;

					void main()
					{
						mat3 normalMatrix = mat3(transpose(inverse(u_Model)));
						v_Normal = normalMatrix * a_Normal;
						v_Tangent = normalMatrix * a_Tangent;
						v_Bitangent = normalMatrix * a_Bitangent;
						v_UV = a_UV;
						gl_Position = u_PVM * vec4(a_Position, 1.0);
					}
				)";

		const std::string frag = R"(
				    #version 410 core
				    layout(location = 0) out vec4 o_Color;
				
				    in vec3 v_Normal;
				    in vec3 v_Tangent;
				    in vec3 v_Bitangent;
				    in vec2 v_UV;
					
					uniform sampler2D u_AlbedoTexture;
					uniform sampler2D u_MetallicTexture;
					uniform sampler2D u_RoughnessTexture;
					uniform sampler2D u_SpecularTexture;
					uniform sampler2D u_NormalTexture;
					uniform sampler2D u_OpacityTexture;
					uniform sampler2D u_EmissiveTexture;
					uniform sampler2D u_AOTexture;

					uniform vec3 u_AmbientColor;
					uniform vec3 u_SunDirection;
					uniform vec3 u_SunColor;
					uniform float u_SunIntensity;

				    void main()
				    {
						vec3 T = normalize(v_Tangent);
						vec3 N = normalize(v_Normal);
						vec3 B = normalize(v_Bitangent);
						mat3 TBN = mat3(T, B, N);


						vec3 normal = texture(u_NormalTexture, v_UV).xyz * 2.0 - 1.0;
						normal = normalize(TBN * normal);

						vec3 lightDir = normalize(u_SunDirection);
						float diff = max(dot(normal, lightDir), 0.0);
						vec3 diffuse = diff * u_SunColor * u_SunIntensity;
						vec3 albedo = texture(u_AlbedoTexture, v_UV).xyz;
						vec3 ambient = u_AmbientColor * albedo;

						o_Color = vec4(albedo * diffuse + ambient, 1.0);
				    }
				)";

		m_DefaultShader.reset( Shader::Create( vert, frag ) );
		m_DefaultMaterial = MakeShared<Material>( );
	}

	void SceneRenderer::Render( const Camera& a_Camera, const Matrix4& a_View )
	{
		Flush();
		BeginScene( a_Camera, a_View );

		// - Submit Draw Calls -
		{
			RenderCommand::SetCullMode( true );
			auto meshComponents = m_Scene->m_Registry.view<StaticMeshComponent, TransformComponent>();
			meshComponents.each( [&]( auto go, StaticMeshComponent& meshComponent, TransformComponent& transform )
				{
					SharedPtr<StaticMesh> staticMesh = AssetManager::GetAsset<StaticMesh>( meshComponent.Mesh );
					if ( !staticMesh )
						return;

					SharedPtr<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>( staticMesh->GetMeshSource() );
					if ( !meshSource )
						return;

					DrawCall drawCall;
					drawCall.VAO = meshSource->GetVAO();

					// If the mesh component has an override material, use that
					if ( meshComponent.Materials.size() > 0 )
					{
						drawCall.Material = meshComponent.Materials[0];
					}
					// Otherwise, use the first material from the mesh source
					else if ( meshSource->GetMaterials().size() > 0 )
					{
						drawCall.Material = meshSource->GetMaterials()[0];
					}

					drawCall.Transform = transform.GetWorldTransform();

					SubmitDrawCall( drawCall );

				} 
			);
		}
		
		m_ViewProjectionMatrix = a_Camera.GetProjection() * a_View;

		// - Perform Draw Calls -
		{
			for ( const auto& drawCall : m_DrawCalls )
			{
				PerformDrawCall( drawCall );
			}
		}

		EndScene();
	}

	void SceneRenderer::BeginScene( const Camera& a_Camera, const Matrix4& a_View )
	{
		RenderCommand::SetClearColor( { 0.1, 0.1, 0.12, 1.0 } );
		RenderCommand::Clear();
	}

	void SceneRenderer::EndScene()
	{
	}

	void SceneRenderer::Flush()
	{
		m_DrawCalls.clear();
	}

	void SceneRenderer::SubmitDrawCall( const DrawCall& a_DrawCall )
	{
		m_DrawCalls.push_back( a_DrawCall );
	}

	void SceneRenderer::PerformDrawCall( const DrawCall& a_DrawCall )
	{
		SharedPtr<Material> material = AssetManager::GetAsset<Material>( a_DrawCall.Material );
		if ( !material )
			material = m_DefaultMaterial;

		SharedPtr<Shader> shader = AssetManager::GetAsset<Shader>( material->Shader );
		if ( !shader )
			shader = m_DefaultShader;

		shader->Bind();
		shader->SetMatrix4( "u_PVM", m_ViewProjectionMatrix * a_DrawCall.Transform );
		shader->SetMatrix4( "u_Model", a_DrawCall.Transform );

		// Bind Uniforms
		shader->SetFloat3( "u_AmbientColor", s_AmbientColor );
		shader->SetFloat3( "u_SunDirection", s_LightDirection );
		shader->SetFloat3( "u_SunColor", s_LightColor );
		shader->SetFloat( "u_SunIntensity", s_LightIntensity );

		// Bind Textures
		{
			if ( auto albedoTexture = AssetManager::GetAsset<Texture>( material->AlbedoTexture ) )
			{
				albedoTexture->Bind( 0 );
				shader->SetInt( "u_AlbedoTexture", 0 );
			}

			if ( auto metallicTexture = AssetManager::GetAsset<Texture>( material->MetallicTexture ) )
			{
				metallicTexture->Bind( 2 );
				shader->SetInt( "u_MetallicTexture", 2 );
			}

			if ( auto roughnessTexture = AssetManager::GetAsset<Texture>( material->RoughnessTexture ) )
			{
				roughnessTexture->Bind( 3 );
				shader->SetInt( "u_RoughnessTexture", 3 );
			}

			if ( auto specularTexture = AssetManager::GetAsset<Texture>( material->SpecularTexture ) )
			{
				specularTexture->Bind( 4 );
				shader->SetInt( "u_SpecularTexture", 4 );
			}

			if ( auto normalTexture = AssetManager::GetAsset<Texture>( material->NormalTexture ) )
			{
				normalTexture->Bind( 5 );
				shader->SetInt( "u_NormalTexture", 5 );
			}

			if ( auto opacityTexture = AssetManager::GetAsset<Texture>( material->OpacityTexture ) )
			{
				opacityTexture->Bind( 6 );
				shader->SetInt( "u_OpacityTexture", 6 );
			}

			if ( auto emissiveTexture = AssetManager::GetAsset<Texture>( material->EmissiveTexture ) )
			{
				emissiveTexture->Bind( 7 );
				shader->SetInt( "u_EmissiveTexture", 7 );
			}

			if ( auto aoTexture = AssetManager::GetAsset<Texture>( material->AOTexture ) )
			{
				aoTexture->Bind( 8 );
				shader->SetInt( "u_AOTexture", 8 );
			}
		}

		a_DrawCall.VAO->Bind();
		RenderCommand::DrawIndexed( a_DrawCall.VAO );
		a_DrawCall.VAO->Unbind();

		shader->Unbind();
	}

}