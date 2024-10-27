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
		// Set Default Assets
		{
			TODO( "Use the asset manager instead." );
			m_DefaultShader.reset( Shader::Create() );
			m_DefaultShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/PBR-Shadows.glsl" );
			m_SkyboxShader.reset( Shader::Create() );
			m_SkyboxShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/EnvironmentMap/SkyBox.glsl" );
			m_DefaultMaterial = MakeShared<Material>();
			m_WhiteTexture = AssetManager::GetAsset<Texture>( TextureFactory::GetWhiteTexture() );
			m_BlackTexture = AssetManager::GetAsset<Texture>( TextureFactory::GetBlackTexture() );
			m_NormalTexture = AssetManager::GetAsset<Texture>( TextureFactory::GetNormalTexture() );
			m_BrdfLUT = TextureLoader::LoadTexture( Application::GetEngineAssetsDirectory() / "Renderer/BRDF_LUT.png" );
		}

		// Initialize Shadow Info
		{

			m_ShadowMapSize = { 1024 * 4, 1024 * 4 };

			m_ShadowMapShader.reset( Shader::Create() );
			m_ShadowMapShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/Shadows/ShadowMap.glsl" );
		}

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

	void SceneRenderer::Render( const SharedPtr<Framebuffer>& a_FBO, const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition )
	{
		Flush();
		BeginScene( a_Camera, a_View, a_CameraPosition );

		// - Submit Directional Lights -
		{
			auto directionalLightComponents = m_Scene->m_Registry.view<DirectionalLightComponent, TransformComponent>();
			uint32_t lightIndex = 0;
			directionalLightComponents.each(
				[&]( auto go, DirectionalLightComponent& lightComponent, TransformComponent& transform )
				{
					if ( lightIndex >= MAX_DIRECTIONAL_LIGHTS )
						return;

					m_LightEnvironment.DirectionalLights[lightIndex].Direction = transform.GetForward();
					m_LightEnvironment.DirectionalLights[lightIndex].Color = lightComponent.LightColor;
					m_LightEnvironment.DirectionalLights[lightIndex].Intensity = lightComponent.Intensity;
					m_LightEnvironment.DirectionalLights[lightIndex].ShadowMap = lightComponent.ShadowMap;

					// TEMP!
					if ( !lightComponent.ShadowMap )
					{
						FramebufferSpecification spec =
						{
							.Width = (uint32_t)m_ShadowMapSize.x,
							.Height = (uint32_t)m_ShadowMapSize.y,
							.Attachments = { EFramebufferTextureFormat::Depth },
						};

						lightComponent.ShadowMap = Framebuffer::Create( spec );
					}

					lightIndex++;
				}
			);
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

		// - Generate Shadow Maps -
		{
			// TEMP!
			m_ShadowFBO = m_LightEnvironment.DirectionalLights[0].ShadowMap;

			// - Directional Light Shadow Pass -
			if ( m_ShadowFBO )
			{
				RenderCommand::SetDepthTest( true );
				RenderCommand::SetCullMode( ECullMode::Front );
				RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );
				RenderCommand::SetViewport( 0, 0, m_ShadowMapSize.x, m_ShadowMapSize.y );
				m_ShadowFBO->Bind();
				RenderCommand::Clear();

				// - Update View Projection Matrix -
				float lightNearPlane = -1000.0f; float lightFarPlane = 1000.0f;
				Matrix4 lightView = glm::lookAt( 
					m_CameraPosition + ( m_LightEnvironment.DirectionalLights[0].Direction * -20.f ),
					m_CameraPosition,
					{ 0.0f, 1.0f, 0.0f } );
				Matrix4 lightProjection = glm::ortho( -35.0f, 35.0f, -35.0f, 35.0f, lightNearPlane, lightFarPlane );
				m_LightViewProjectionMatrix = lightProjection * lightView;

				m_ShadowMapShader->Bind();

				for ( const auto& drawCall : m_DrawCalls )
				{
					m_ShadowMapShader->SetMatrix4( "u_PVM", m_LightViewProjectionMatrix * drawCall.Transform );

					drawCall.VAO->Bind();
					RenderCommand::DrawIndexed( drawCall.VAO );
					drawCall.VAO->Unbind();
				}

				m_ShadowMapShader->Unbind();

				m_ShadowFBO->Unbind();
			}
		}

		a_FBO->Bind();
		BeginScene( a_Camera, a_View, a_CameraPosition ); // Temp ?

		// - Reset Viewport -
		RenderCommand::SetViewport( 0, 0, a_Camera.GetViewportSize().x, a_Camera.GetViewportSize().y );
		
		// - Update View Projection Matrix -
		m_ViewMatrix = a_View;
		m_ProjectionMatrix = a_Camera.GetProjection();
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

		// - Geometry Pass -
		{
			RenderCommand::SetDepthTest( true );
			RenderCommand::SetCullMode( ECullMode::Back );
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

		a_FBO->Unbind();

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

		#pragma region Bind Uniforms

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
		auto albedoTexture = AssetManager::GetAsset<Texture>( material->AlbedoTexture );
		if ( !albedoTexture ) albedoTexture = m_WhiteTexture;
		albedoTexture->Bind( 0 );
		shader->SetInt( "u_AlbedoTexture", 0 );

		auto metallicTexture = AssetManager::GetAsset<Texture>( material->MetallicTexture );
		if ( !metallicTexture ) metallicTexture = m_BlackTexture;
		metallicTexture->Bind( 1 );
		shader->SetInt( "u_MetallicTexture", 1 );

		auto roughnessTexture = AssetManager::GetAsset<Texture>( material->RoughnessTexture );
		if ( !roughnessTexture ) roughnessTexture = m_WhiteTexture;
		roughnessTexture->Bind( 2 );
		shader->SetInt( "u_RoughnessTexture", 2 );

		auto normalTexture = AssetManager::GetAsset<Texture>( material->NormalTexture );
		if ( !normalTexture ) normalTexture = m_NormalTexture;
		normalTexture->Bind( 3 );
		shader->SetInt( "u_NormalTexture", 3 );

		//auto opacityTexture = AssetManager::GetAsset<Texture>( material->OpacityTexture );
		//if ( !opacityTexture ) opacityTexture = m_WhiteTexture;
		//opacityTexture->Bind( 4 );
		//shader->SetInt( "u_OpacityTexture", 4 );

		auto emissiveTexture = AssetManager::GetAsset<Texture>( material->EmissiveTexture );
		if ( !emissiveTexture ) emissiveTexture = m_BlackTexture;
		emissiveTexture->Bind( 5 );
		shader->SetInt( "u_EmissiveTexture", 5 );

		auto aoTexture = AssetManager::GetAsset<Texture>( material->AOTexture );
		if ( !aoTexture ) aoTexture = m_WhiteTexture;
		aoTexture->Bind( 6 );
		shader->SetInt( "u_AOTexture", 6 );

		// Bind environment map
		if ( m_SceneEnvironment.HDRI.EnvironmentMap && m_SceneEnvironment.HDRI.EnvironmentMap->GetIrradianceMap() )
		{
			if ( auto radianceMap = m_SceneEnvironment.HDRI.EnvironmentMap->GetRadianceMap() )
			{
				radianceMap->Bind( 7 );
				shader->SetInt( "u_Environment.PrefilterMap", 7 );
			}

			shader->SetFloat( "u_Environment.Roughness", m_SceneEnvironment.HDRI.Blur );
			shader->SetFloat( "u_Environment.Exposure", m_SceneEnvironment.HDRI.Exposure );
			shader->SetFloat( "u_Environment.Gamma", m_SceneEnvironment.HDRI.Gamma );
			shader->SetFloat( "u_Environment.Intensity", m_SceneEnvironment.HDRI.Intensity );
		}

		// Bind BRDF LUT
		if ( m_BrdfLUT )
		{
			m_BrdfLUT->Bind( 8 );
			shader->SetInt( "u_Environment.BrdfLUT", 8 );
		}

		#pragma endregion

		// Temp ?
		if ( m_ShadowFBO ) m_ShadowFBO->BindDepthAttatchment( 9 );
		shader->SetInt( "u_ShadowMap", 9 );
		shader->SetMatrix4( "u_LightSpaceMatrix", m_LightViewProjectionMatrix );
		shader->SetFloat3( "u_LightPos", m_LightEnvironment.DirectionalLights[0].Direction );
		shader->SetFloat3( "u_ViewPos", m_CameraPosition );


		a_DrawCall.VAO->Bind();
		RenderCommand::DrawIndexed( a_DrawCall.VAO );
		a_DrawCall.VAO->Unbind();

		#pragma region Unbind Uniforms

		albedoTexture->Unbind(0);
		metallicTexture->Unbind(1);
		roughnessTexture->Unbind(2);
		normalTexture->Unbind(3);
		//opacityTexture->Unbind(4);
		emissiveTexture->Unbind(5);
		aoTexture->Unbind(6);

		if ( m_ShadowFBO ) m_ShadowFBO->UnbindDepthAttatchment(9);

		if ( m_SceneEnvironment.HDRI.EnvironmentMap && m_SceneEnvironment.HDRI.EnvironmentMap->GetIrradianceMap() )
			m_SceneEnvironment.HDRI.EnvironmentMap->GetIrradianceMap()->Unbind();

		#pragma endregion

		shader->Unbind();
	}

}