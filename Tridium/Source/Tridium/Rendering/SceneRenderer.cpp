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
		m_DefaultShader.reset( Shader::Create() );
		m_DefaultShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/Default.glsl" );
		m_DefaultMaterial = MakeShared<Material>( );
	}

	void SceneRenderer::Render( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition )
	{
		Flush();
		BeginScene( a_Camera, a_View, a_CameraPosition );

		// - Submit Point Lights -
		{
			auto pointLightComponents = m_Scene->m_Registry.view<PointLightComponent, TransformComponent>();
			uint32_t lightIndex = 0;
			pointLightComponents.each( 
				[&]( auto go, PointLightComponent& lightComponent, TransformComponent& transform )
				{
					if ( lightIndex >= MAX_POINT_LIGHTS )
						return;

					m_PointLights[lightIndex].Position = transform.Position;
					m_PointLights[lightIndex].Color = lightComponent.Color;
					m_PointLights[lightIndex].Intensity = lightComponent.Intensity;
					lightIndex++;
				}
			);
		}

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

		// - Perform Mesh Draw Calls -
		{
			for ( const auto& drawCall : m_DrawCalls )
			{
				PerformDrawCall( drawCall );
			}
		}

		EndScene();
	}

	void SceneRenderer::BeginScene( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition )
	{
		RenderCommand::SetClearColor( { 0.1, 0.1, 0.12, 1.0 } );
		RenderCommand::Clear();

		m_CameraPosition = a_CameraPosition;
	}

	void SceneRenderer::EndScene()
	{
	}

	void SceneRenderer::Flush()
	{
		m_DrawCalls.clear();
		for ( auto& pointLight : m_PointLights )
		{
			pointLight = {};
		}
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

		// Bind Uniforms
		{
			shader->Bind();
			shader->SetMatrix4( "u_PVM", m_ViewProjectionMatrix * a_DrawCall.Transform );
			shader->SetMatrix4( "u_Model", a_DrawCall.Transform );
			shader->SetFloat3( "u_CameraPosition", m_CameraPosition );
			// Bind Point Lights
			for ( uint32_t i = 0; i < MAX_POINT_LIGHTS; i++ )
			{
				std::string index = std::to_string( i );
				shader->SetFloat3( ( "u_PointLights[" + index + "].Position" ).c_str(), m_PointLights[i].Position );
				shader->SetFloat3( ( "u_PointLights[" + index + "].Color" ).c_str(), m_PointLights[i].Color );
				shader->SetFloat( ( "u_PointLights[" + index + "].Intensity" ).c_str(), m_PointLights[i].Intensity );
			}
		}

		// Bind Textures
		{
			if ( auto albedoTexture = AssetManager::GetAsset<Texture>( material->AlbedoTexture ) )
			{
				albedoTexture->Bind( 0 );
				shader->SetInt( "u_AlbedoTexture", 0 );
			}

			if ( auto metallicTexture = AssetManager::GetAsset<Texture>( material->MetallicTexture ) )
			{
				metallicTexture->Bind( 1 );
				shader->SetInt( "u_MetallicTexture", 1 );
			}

			if ( auto roughnessTexture = AssetManager::GetAsset<Texture>( material->RoughnessTexture ) )
			{
				roughnessTexture->Bind( 2 );
				shader->SetInt( "u_RoughnessTexture", 2 );
			}

			if ( auto specularTexture = AssetManager::GetAsset<Texture>( material->SpecularTexture ) )
			{
				specularTexture->Bind( 3 );
				shader->SetInt( "u_SpecularTexture", 3 );
			}

			if ( auto normalTexture = AssetManager::GetAsset<Texture>( material->NormalTexture ) )
			{
				normalTexture->Bind( 4 );
				shader->SetInt( "u_NormalTexture", 4 );
			}

			if ( auto opacityTexture = AssetManager::GetAsset<Texture>( material->OpacityTexture ) )
			{
				opacityTexture->Bind( 5 );
				shader->SetInt( "u_OpacityTexture", 5 );
			}

			if ( auto emissiveTexture = AssetManager::GetAsset<Texture>( material->EmissiveTexture ) )
			{
				emissiveTexture->Bind( 6 );
				shader->SetInt( "u_EmissiveTexture", 6 );
			}

			if ( auto aoTexture = AssetManager::GetAsset<Texture>( material->AOTexture ) )
			{
				aoTexture->Bind( 7 );
				shader->SetInt( "u_AOTexture", 7 );
			}
		}

		a_DrawCall.VAO->Bind();
		RenderCommand::DrawIndexed( a_DrawCall.VAO );
		a_DrawCall.VAO->Unbind();

		shader->Unbind();
	}

}