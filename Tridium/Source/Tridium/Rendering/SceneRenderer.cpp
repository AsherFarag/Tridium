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
#include <Tridium/Rendering/EnvironmentMap.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>

namespace Tridium {

	SceneRenderer::SceneRenderer( const SharedPtr<Scene>& a_Scene )
		: m_Scene( a_Scene ), m_SceneEnvironment( a_Scene->GetSceneEnvironment() )
	{
		TODO( "Use the asset manager instead." );
		m_DefaultShader.reset( Shader::Create() );
		m_DefaultShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/Default.glsl" );
		m_SkyboxShader.reset( Shader::Create() );
		m_SkyboxShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/EnvironmentMap/SkyBox.glsl" );
		m_DefaultMaterial = MakeShared<Material>( );

		m_BrdfLUT = TextureLoader::LoadTexture( Application::GetEngineAssetsDirectory() / "Renderer/BRDF_LUT.png" );

		// Create the skybox VAO
		{
			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};

			uint32_t indicies[] = {
				0, 1, 2, 3, 4, 5,
				6, 7, 8, 9, 10, 11,
				12, 13, 14, 15, 16, 17,
				18, 19, 20, 21, 22, 23,
				24, 25, 26, 27, 28, 29,
				30, 31, 32, 33, 34, 35
			};

			m_SkyboxVAO = VertexArray::Create();
			SharedPtr<VertexBuffer> vbo = VertexBuffer::Create( skyboxVertices, sizeof( skyboxVertices ) );
			BufferLayout layout = {
				{ ShaderDataType::Float3, "a_Position" }
			};
			vbo->SetLayout( layout );

			m_SkyboxVAO->AddVertexBuffer( vbo );
			m_SkyboxVAO->SetIndexBuffer( IndexBuffer::Create( indicies, sizeof( indicies ) / sizeof( uint32_t ) ) );
		}
	}

	void SceneRenderer::Render( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition )
	{
		Flush();
		BeginScene( a_Camera, a_View, a_CameraPosition );

		// - Submit Directional Lights -
		{
			m_LightEnvironment.DirectionalLights[0] = m_SceneEnvironment.DirectionalLight;
			// Convert angles from degrees to radians
			Vector3 eulerRadians = glm::radians( m_SceneEnvironment.DirectionalLight.Direction );
			// Calculate direction vector
			m_LightEnvironment.DirectionalLights[0].Direction.x = cos( eulerRadians.x ) * cos( eulerRadians.y );
			m_LightEnvironment.DirectionalLights[0].Direction.y = sin( eulerRadians.y );
			m_LightEnvironment.DirectionalLights[0].Direction.z = sin( eulerRadians.x ) * cos( eulerRadians.y );
		}

		// - Submit Point Lights -
		{
			auto pointLightComponents = m_Scene->m_Registry.view<PointLightComponent, TransformComponent>();
			uint32_t lightIndex = 0;
			pointLightComponents.each( 
				[&]( auto go, PointLightComponent& lightComponent, TransformComponent& transform )
				{
					if ( lightIndex >= MAX_POINT_LIGHTS )
						return;

					m_LightEnvironment.PointLights[lightIndex].Position = transform.Position;
					m_LightEnvironment.PointLights[lightIndex].Color = lightComponent.LightColor;
					m_LightEnvironment.PointLights[lightIndex].Intensity = lightComponent.Intensity;
					m_LightEnvironment.PointLights[lightIndex].FalloffExponent = lightComponent.FalloffExponent;
					m_LightEnvironment.PointLights[lightIndex].AttenuationRadius = lightComponent.AttenuationRadius;
					lightIndex++;
				}
			);
		}

		// - Submit Spot Lights -
		{
			auto spotLightComponents = m_Scene->m_Registry.view<SpotLightComponent, TransformComponent>();
			uint32_t lightIndex = 0;
			spotLightComponents.each(
				[&]( auto go, SpotLightComponent& lightComponent, TransformComponent& transform )
				{
					if ( lightIndex >= MAX_SPOT_LIGHTS )
						return;

					m_LightEnvironment.SpotLights[lightIndex].Position = transform.Position;
					m_LightEnvironment.SpotLights[lightIndex].Direction = transform.GetForward();
					m_LightEnvironment.SpotLights[lightIndex].Color = lightComponent.LightColor;
					m_LightEnvironment.SpotLights[lightIndex].Intensity = lightComponent.Intensity;
					m_LightEnvironment.SpotLights[lightIndex].FalloffExponent = lightComponent.FalloffExponent;
					m_LightEnvironment.SpotLights[lightIndex].AttenuationRadius = lightComponent.AttenuationRadius;
					m_LightEnvironment.SpotLights[lightIndex].InnerConeAngle =  glm::radians( lightComponent.InnerConeAngle );
					m_LightEnvironment.SpotLights[lightIndex].OuterConeAngle = glm::radians( lightComponent.OuterConeAngle );
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
		
		// - Update View Projection Matrix -
		m_ViewMatrix = a_View;
		m_ProjectionMatrix = a_Camera.GetProjection();
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

		// - Perform Mesh Draw Calls -
		{
			RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );
			for ( const auto& drawCall : m_DrawCalls )
			{
				PerformDrawCall( drawCall );
			}
		}

		// - Draw Skybox -
		if ( m_SceneEnvironment.HDRI.EnvironmentMap && m_SceneEnvironment.HDRI.EnvironmentMap->GetRadianceMap() )
		{
			auto radianceMap = m_SceneEnvironment.HDRI.EnvironmentMap->GetRadianceMap();
			RenderCommand::SetDepthCompare( EDepthCompareOperator::LessOrEqual );
			m_SkyboxShader->Bind();
			{
				Matrix4 skyboxView = Matrix4( Matrix3( m_ViewMatrix ) ); // Remove translation
				Matrix4 skyboxTransform = glm::scale( Matrix4( 1.0f ), { 100.0f, 100.0f, 100.0f } );

				m_SkyboxShader->SetMatrix4( "u_Projection", m_ProjectionMatrix );
				m_SkyboxShader->SetMatrix4( "u_View", skyboxView );
				m_SkyboxShader->SetMatrix4( "u_Model", skyboxTransform );
				m_SkyboxShader->SetFloat( "u_Exposure", m_SceneEnvironment.HDRI.Exposure );
				m_SkyboxShader->SetFloat( "u_Gamma", m_SceneEnvironment.HDRI.Gamma );
				const uint32_t MaxMipLevels = 5;
				float roughness = static_cast<float>( m_SceneEnvironment.HDRI.Blur * ( MaxMipLevels - 1 ) );
				m_SkyboxShader->SetFloat( "u_Roughness", roughness );

				radianceMap->Bind();

				m_SkyboxVAO->Bind();
				RenderCommand::DrawIndexed( m_SkyboxVAO );
				m_SkyboxVAO->Unbind();

				radianceMap->Unbind();
			}
			m_SkyboxShader->Unbind();
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
		for ( auto& pointLight : m_LightEnvironment.PointLights )
		{
			pointLight = {};
		}
		for ( auto& spotLight : m_LightEnvironment.SpotLights )
		{
			spotLight = {};
		}
		for ( auto& directionalLight : m_LightEnvironment.DirectionalLights )
		{
			directionalLight = {};
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
				PointLight& pointLight = m_LightEnvironment.PointLights[i];
				std::string index = std::to_string( i );
				shader->SetFloat3( ( "u_PointLights[" + index + "].Position" ).c_str(), pointLight.Position );
				shader->SetFloat3( ( "u_PointLights[" + index + "].Color" ).c_str(), pointLight.Color );
				shader->SetFloat( ( "u_PointLights[" + index + "].Intensity" ).c_str(), pointLight.Intensity );
				shader->SetFloat( ( "u_PointLights[" + index + "].FalloffExponent" ).c_str(), pointLight.FalloffExponent );
				shader->SetFloat( ( "u_PointLights[" + index + "].AttenuationRadius" ).c_str(), pointLight.AttenuationRadius );
			}

			// Bind Spot Lights
			for ( uint32_t i = 0; i < MAX_SPOT_LIGHTS; ++i )
			{
				SpotLight& spotLight = m_LightEnvironment.SpotLights[i];
				std::string index = std::to_string( i );
				shader->SetFloat3( ( "u_SpotLights[" + index + "].Position" ).c_str(), spotLight.Position );
				shader->SetFloat3( ( "u_SpotLights[" + index + "].Direction" ).c_str(), spotLight.Direction );
				shader->SetFloat3( ( "u_SpotLights[" + index + "].Color" ).c_str(), spotLight.Color );
				shader->SetFloat( ( "u_SpotLights[" + index + "].Intensity" ).c_str(), spotLight.Intensity );
				shader->SetFloat( ( "u_SpotLights[" + index + "].FalloffExponent" ).c_str(), spotLight.FalloffExponent );
				shader->SetFloat( ( "u_SpotLights[" + index + "].AttenuationRadius" ).c_str(), spotLight.AttenuationRadius );
				shader->SetFloat( ( "u_SpotLights[" + index + "].InnerConeAngle" ).c_str(), spotLight.InnerConeAngle );
				shader->SetFloat( ( "u_SpotLights[" + index + "].OuterConeAngle" ).c_str(), spotLight.OuterConeAngle );
			}

			// Bind Directional Lights
			for ( uint32_t i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i )
			{
				DirectionalLight& directionalLight = m_LightEnvironment.DirectionalLights[i];
				std::string index = std::to_string( i );
				shader->SetFloat3( ( "u_DirectionalLights[" + index + "].Direction" ).c_str(), directionalLight.Direction );
				shader->SetFloat3( ( "u_DirectionalLights[" + index + "].Color" ).c_str(), directionalLight.Color );
				shader->SetFloat( ( "u_DirectionalLights[" + index + "].Intensity" ).c_str(), directionalLight.Intensity );
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

			// Bind environment map
			if ( m_SceneEnvironment.HDRI.EnvironmentMap && m_SceneEnvironment.HDRI.EnvironmentMap->GetIrradianceMap() )
			{
				if ( auto irradianceMap = m_SceneEnvironment.HDRI.EnvironmentMap->GetIrradianceMap() )
				{
					irradianceMap->Bind( 8 );
					shader->SetInt( "u_Environment.IrradianceMap", 8 );
				}

				if ( auto radianceMap = m_SceneEnvironment.HDRI.EnvironmentMap->GetRadianceMap() )
				{
					radianceMap->Bind( 9 );
					shader->SetInt( "u_Environment.PrefilterMap", 9 );
				}

				shader->SetFloat( "u_Environment.Roughness", m_SceneEnvironment.HDRI.Blur );
				shader->SetFloat( "u_Environment.Exposure", m_SceneEnvironment.HDRI.Exposure );
				shader->SetFloat( "u_Environment.Gamma", m_SceneEnvironment.HDRI.Gamma );
			}

			// Bind BRDF LUT
			if ( m_BrdfLUT )
			{
				m_BrdfLUT->Bind( 10 );
				shader->SetInt( "u_Environment.BrdfLUT", 10 );
			}
		}

		a_DrawCall.VAO->Bind();
		RenderCommand::DrawIndexed( a_DrawCall.VAO );
		a_DrawCall.VAO->Unbind();

		// Unbind Textures
		{
			if ( auto albedoTexture = AssetManager::GetAsset<Texture>( material->AlbedoTexture ) )
				albedoTexture->Unbind();

			if ( auto metallicTexture = AssetManager::GetAsset<Texture>( material->MetallicTexture ) )
				metallicTexture->Unbind();

			if ( auto roughnessTexture = AssetManager::GetAsset<Texture>( material->RoughnessTexture ) )
				roughnessTexture->Unbind();

			if ( auto specularTexture = AssetManager::GetAsset<Texture>( material->SpecularTexture ) )
				specularTexture->Unbind();

			if ( auto normalTexture = AssetManager::GetAsset<Texture>( material->NormalTexture ) )
				normalTexture->Unbind();

			if ( auto opacityTexture = AssetManager::GetAsset<Texture>( material->OpacityTexture ) )
				opacityTexture->Unbind();

			if ( auto emissiveTexture = AssetManager::GetAsset<Texture>( material->EmissiveTexture ) )
				emissiveTexture->Unbind();

			if ( auto aoTexture = AssetManager::GetAsset<Texture>( material->AOTexture ) )
				aoTexture->Unbind();

			if ( m_SceneEnvironment.HDRI.EnvironmentMap && m_SceneEnvironment.HDRI.EnvironmentMap->GetIrradianceMap() )
				m_SceneEnvironment.HDRI.EnvironmentMap->GetIrradianceMap()->Unbind();
		}

		shader->Unbind();
	}

}