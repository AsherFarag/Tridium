#include "tripch.h"
#include "SceneRenderer.h"
#include <Tridium/Scene/Scene.h>
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

	SceneRenderer::SceneRenderer( Scene& a_Scene )
		: m_Scene( a_Scene ), m_SceneEnvironment( a_Scene.GetSceneEnvironment() )
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

		// Initalize Deferred Data
		{
			m_DeferredData.GBufferShader.reset( Shader::Create() );
			m_DeferredData.GBufferShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/Deferred/GBuffer.glsl" );

			m_DeferredData.LightingShader.reset( Shader::Create() );
			m_DeferredData.LightingShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/Deferred/DeferredPBR.glsl" );

			// Create Quad VAO
			{
				BufferLayout layout =
				{
					{ EShaderDataType::Float3, "a_Position" },
					{ EShaderDataType::Float2, "a_UV" }
				};

				float vertices[] =
				{
					-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
					1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
					1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
					-1.0f, 1.0f, 0.0f, 0.0f, 1.0f
				};

				uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

				m_DeferredData.QuadVAO = VertexArray::Create();
				SharedPtr<VertexBuffer> vertexBuffer = VertexBuffer::Create( vertices, sizeof( vertices ) );
				vertexBuffer->SetLayout( layout );
				m_DeferredData.QuadVAO->AddVertexBuffer( vertexBuffer );
				SharedPtr<IndexBuffer> indexBuffer = IndexBuffer::Create( indices, sizeof( indices ) / sizeof( uint32_t ) );
				m_DeferredData.QuadVAO->SetIndexBuffer( indexBuffer );
			}
		}

		// Initialize Shadow Info
		{
			m_ShadowMapSize = { 1024 * 8, 1024 * 8 };
			m_ShadowMapShader.reset( Shader::Create() );
			m_ShadowMapShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/Shadows/ShadowMap.glsl" );
			m_ShadowCubeMapShader.reset( Shader::Create() );
			m_ShadowCubeMapShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/Shadows/CubeShadowMap.glsl" );
		}

		// Create Cube Mesh
		{
			m_CubeMesh = MeshFactory::CreateCube();
		}
	}

	void SceneRenderer::Render( const SharedPtr<Framebuffer>& a_FBO, const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition )
	{
		Clear();
		m_RenderTarget = a_FBO;

		BeginScene( a_Camera, a_View, a_CameraPosition );

		// - Generate Shadow Maps -
		{
			GenerateShadowMaps();
		}

		// - Render Pass -
		switch ( m_RenderMode )
		{
			case ERenderMode::Forward:
			{
				ForwardRenderPass();
				break;
			}
			case ERenderMode::Deferred:
			{
				DeferredRenderPass();
				break;
			}
			default:
			{
				TE_CORE_ASSERT( false, "Unknown render mode" );
				break;
			}
		}

		EndScene();
	}

	void SceneRenderer::BeginScene( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition )
	{
		m_SceneInfo = SceneInfo
		{
			.ProjectionMatrix = a_Camera.GetProjection(),
			.ViewMatrix = a_View,
			.ViewProjectionMatrix = a_Camera.GetProjection() * a_View,
			.CameraPosition = a_CameraPosition,
			.Camera = a_Camera,
		};

		// - Submit Directional Lights -
		{
			auto directionalLightComponents = m_Scene.m_Registry.view<DirectionalLightComponent, TransformComponent>();
			uint32_t lightIndex = 0;
			directionalLightComponents.each(
				[&]( auto go, DirectionalLightComponent& lightComponent, TransformComponent& transform )
				{
					if ( lightIndex >= MAX_DIRECTIONAL_LIGHTS )
						return;

					// TEMP!
					if ( lightComponent.CastsShadows && !lightComponent.ShadowMap )
					{
						FramebufferSpecification spec =
						{
							.Width = (uint32_t)m_ShadowMapSize.x,
							.Height = (uint32_t)m_ShadowMapSize.y,
							.Attachments = { EFramebufferTextureFormat::Depth },
						};

						lightComponent.ShadowMap = Framebuffer::Create( spec );
					}

					if ( !lightComponent.CastsShadows )
					{
						lightComponent.ShadowMap.reset();
					}

					m_LightEnvironment.DirectionalLights[lightIndex].Direction = transform.GetForward();
					m_LightEnvironment.DirectionalLights[lightIndex].Color = lightComponent.LightColor;
					m_LightEnvironment.DirectionalLights[lightIndex].Intensity = lightComponent.Intensity;
					m_LightEnvironment.DirectionalLights[lightIndex].CastsShadows = lightComponent.CastsShadows;
					m_LightEnvironment.DirectionalLights[lightIndex].ShadowMap = lightComponent.ShadowMap;

					lightIndex++;
				}
			);
		}

		// - Submit Point Lights -
		{
			auto pointLightComponents = m_Scene.m_Registry.view<PointLightComponent, TransformComponent>();
			uint32_t lightIndex = 0;
			pointLightComponents.each(
				[&]( auto go, PointLightComponent& lightComponent, TransformComponent& transform )
				{
					if ( lightIndex >= MAX_POINT_LIGHTS )
						return;

					// TEMP!
					if ( lightComponent.CastsShadows && !lightComponent.ShadowMap )
					{
						FramebufferSpecification spec =
						{
							.Width = (uint32_t)m_ShadowMapSize.x,
							.Height = (uint32_t)m_ShadowMapSize.y,
							.Attachments = { EFramebufferTextureFormat::Depth },
						};

						lightComponent.ShadowMap = Framebuffer::Create( spec );
					}

					if ( !lightComponent.CastsShadows )
					{
						lightComponent.ShadowMap.reset();
					}

					m_LightEnvironment.PointLights[lightIndex].Position = transform.GetWorldPosition();
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
			auto spotLightComponents = m_Scene.m_Registry.view<SpotLightComponent, TransformComponent>();
			uint32_t lightIndex = 0;
			spotLightComponents.each(
				[&]( auto go, SpotLightComponent& lightComponent, TransformComponent& transform )
				{
					if ( lightIndex >= MAX_SPOT_LIGHTS )
						return;

					m_LightEnvironment.SpotLights[lightIndex].Position = transform.GetWorldPosition();
					m_LightEnvironment.SpotLights[lightIndex].Direction = transform.GetForward();
					m_LightEnvironment.SpotLights[lightIndex].Color = lightComponent.LightColor;
					m_LightEnvironment.SpotLights[lightIndex].Intensity = lightComponent.Intensity;
					m_LightEnvironment.SpotLights[lightIndex].FalloffExponent = lightComponent.FalloffExponent;
					m_LightEnvironment.SpotLights[lightIndex].AttenuationRadius = lightComponent.AttenuationRadius;
					m_LightEnvironment.SpotLights[lightIndex].InnerConeAngle = glm::radians( lightComponent.InnerConeAngle );
					m_LightEnvironment.SpotLights[lightIndex].OuterConeAngle = glm::radians( lightComponent.OuterConeAngle );
					lightIndex++;
				}
			);
		}

		// - Submit Draw Calls -
		{
			auto meshComponents = m_Scene.m_Registry.view<StaticMeshComponent, TransformComponent>();
			meshComponents.each( [&]( auto go, StaticMeshComponent& meshComponent, TransformComponent& transform )
				{
					SharedPtr<StaticMesh> staticMesh = AssetManager::GetAsset<StaticMesh>( meshComponent.Mesh );
					if ( !staticMesh )
						return;

					SharedPtr<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>( staticMesh->GetMeshSource() );
					if ( !meshSource )
						return;

					Matrix4 worldTransform = transform.GetWorldTransform();

					for ( uint32_t submeshIndex : staticMesh->GetSubMeshes() )
					{
						if ( submeshIndex >= meshSource->GetSubMeshes().size() )
						{
							TE_CORE_ASSERT( false, "Submesh index out of bounds" );
							continue;
						}

						const SubMesh& submesh = meshSource->GetSubMeshes()[submeshIndex];

						MaterialHandle material = MaterialHandle::InvalidGUID;
						if ( submesh.MaterialIndex < meshSource->GetMaterials().size() )
							material = meshSource->GetMaterials()[submesh.MaterialIndex];

						DrawCall drawCall =
						{
							.VAO = submesh.VAO,
							.Material = material,
							.Transform = worldTransform * submesh.Transform,
						};

						// Override material
						if ( meshComponent.Materials.size() > submesh.MaterialIndex )
							drawCall.Material = meshComponent.Materials[submesh.MaterialIndex];

						SubmitDrawCall( std::move( drawCall ) );
					}
				}
			);
		}
	}

	void SceneRenderer::EndScene()
	{
		// Release ownership of the render target
		m_RenderTarget.reset();
	}

	void SceneRenderer::Clear()
	{
		m_DrawCalls.clear();

		for ( auto& pointLight : m_LightEnvironment.PointLights )
			pointLight = {};

		for ( auto& spotLight : m_LightEnvironment.SpotLights )
			spotLight = {};

		for ( auto& directionalLight : m_LightEnvironment.DirectionalLights )
			directionalLight = {};
	}

	void SceneRenderer::SubmitDrawCall( DrawCall&& a_DrawCall )
	{
		m_DrawCalls.emplace_back( a_DrawCall );
	}

	//////////////////////////////////////////////////////////////////////////
	// Shadows
	//////////////////////////////////////////////////////////////////////////

	void SceneRenderer::GenerateShadowMaps()
	{
		RenderCommand::SetDepthTest( true );

		TODO( "Front face culling only works on opaque objects, need to handle transparent objects by using a bias." );
		// Set Cull Mode to front to avoid peter panning
		RenderCommand::SetCullMode( ECullMode::Front );
		RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );
		RenderCommand::SetViewport( 0, 0, m_ShadowMapSize.x, m_ShadowMapSize.y );

		// Generate Directional Light Shadow Maps
		{
			m_ShadowMapShader->Bind();

			for ( auto& directionalLight : m_LightEnvironment.DirectionalLights )
			{
				if ( !directionalLight.CastsShadows )
					continue;

				directionalLight.ShadowMap->Bind();
				RenderCommand::Clear();

				// - Update View Projection Matrix -
				float lightNearPlane = -1000.0f; float lightFarPlane = 1000.0f;
				Matrix4 lightView = glm::lookAt(
					m_SceneInfo.CameraPosition + ( m_LightEnvironment.DirectionalLights[0].Direction * -20.f ),
					m_SceneInfo.CameraPosition,
					{ 0.0f, 1.0f, 0.0f } );
				Matrix4 lightProjection = glm::ortho( -35.0f, 35.0f, -35.0f, 35.0f, lightNearPlane, lightFarPlane );
				m_LightViewProjectionMatrix = lightProjection * lightView;

				for ( const auto& drawCall : m_DrawCalls )
				{
					m_ShadowMapShader->SetMatrix4( "u_PVM", m_LightViewProjectionMatrix * drawCall.Transform );

					drawCall.VAO->Bind();
					RenderCommand::DrawIndexed( drawCall.VAO );
					drawCall.VAO->Unbind();
				}

				directionalLight.ShadowMap->Unbind();
			}

			m_ShadowMapShader->Unbind();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Deferred Rendering
	//////////////////////////////////////////////////////////////////////////

	void SceneRenderer::DeferredRenderPass()
	{
		const iVector2 viewportSize = m_SceneInfo.Camera.GetViewportSize();
		// - GBuffer Pass -
		{
			m_DeferredData.GBuffer.Resize( viewportSize.x, viewportSize.y );
			m_DeferredData.GBuffer.Bind();
			RenderCommand::Clear();
			RenderCommand::SetBlendmode( EBlendMode::None );

			// - Reset Viewport -
			RenderCommand::SetViewport( 0, 0, viewportSize.x, viewportSize.y );

			// Geometry Pass
			{
				DeferredGBufferPass();
			}

			m_DeferredData.GBuffer.Unbind();
		}

		m_RenderTarget->Resize( viewportSize.x, viewportSize.y );
		m_RenderTarget->Bind();
		RenderCommand::Clear();

		// - Lighting Pass -
		{
			DeferredLightingPass();
		}

		// Copy Depth Buffer
		{
			m_DeferredData.GBuffer.GetFramebuffer()->BlitTo( 
				m_RenderTarget,
				{ 0, 0 }, { viewportSize.x, viewportSize.y }, // Source min, max
				{ 0, 0 }, { viewportSize.x, viewportSize.y }, // Destination min, max
				EFramebufferTextureFormat::Depth, ETextureFilter::Nearest );
		}

		m_RenderTarget->Bind();

		// - Draw Skybox -
		{
			RenderSkybox();
		}

		// - Apply Post-Processing Pass -
		{
			PostProcessPass();
		}

		m_RenderTarget->Unbind();
	}

	void SceneRenderer::DeferredGBufferPass()
	{
		RenderCommand::SetDepthTest( true );
		RenderCommand::SetCullMode( ECullMode::Back );
		RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );

		m_DeferredData.GBufferShader->Bind();
		MaterialHandle lastMaterial = MaterialHandle::InvalidGUID;

		auto bindMaterial = [&]( SharedPtr<Material>& material, SharedPtr<Shader>& shader )
			{
				// Bind Textures
				uint32_t textureSlot = 0;
				auto albedoTexture = AssetManager::GetAsset<Texture>( material->AlbedoTexture );
				if ( !albedoTexture ) albedoTexture = m_WhiteTexture;
				albedoTexture->Bind( textureSlot );
				shader->SetInt( "u_AlbedoTexture", textureSlot );

				textureSlot++;
				auto metallicTexture = AssetManager::GetAsset<Texture>( material->MetallicTexture );
				if ( !metallicTexture ) metallicTexture = m_BlackTexture;
				metallicTexture->Bind( textureSlot );
				shader->SetInt( "u_MetallicTexture", textureSlot );

				textureSlot++;
				auto roughnessTexture = AssetManager::GetAsset<Texture>( material->RoughnessTexture );
				if ( !roughnessTexture ) roughnessTexture = m_WhiteTexture;
				roughnessTexture->Bind( textureSlot );
				shader->SetInt( "u_RoughnessTexture", textureSlot );

				textureSlot++;
				auto normalTexture = AssetManager::GetAsset<Texture>( material->NormalTexture );
				if ( !normalTexture ) normalTexture = m_NormalTexture;
				normalTexture->Bind( textureSlot );
				shader->SetInt( "u_NormalTexture", textureSlot );

				textureSlot++;
				TODO( "Should we be setting the opacity texture to the albedo if there isn't an opacity texture?" );
				auto opacityTexture = AssetManager::GetAsset<Texture>( material->OpacityTexture );
				if ( !opacityTexture ) opacityTexture = albedoTexture;
				opacityTexture->Bind( textureSlot );
				shader->SetInt( "u_OpacityTexture", textureSlot );

				textureSlot++;
				auto emissiveTexture = AssetManager::GetAsset<Texture>( material->EmissiveTexture );
				if ( !emissiveTexture ) emissiveTexture = m_BlackTexture;
				emissiveTexture->Bind( textureSlot );
				shader->SetInt( "u_EmissiveTexture", textureSlot );

				textureSlot++;
				auto aoTexture = AssetManager::GetAsset<Texture>( material->AOTexture );
				if ( !aoTexture ) aoTexture = m_WhiteTexture;
				aoTexture->Bind( textureSlot );
				shader->SetInt( "u_AOTexture", textureSlot );
			};

		bindMaterial( m_DefaultMaterial, m_DeferredData.GBufferShader );

		for ( const auto& drawCall : m_DrawCalls )
		{
			if ( drawCall.Material != lastMaterial )
			{
				auto material = AssetManager::GetAsset<Material>( drawCall.Material );
				if ( !material ) material = m_DefaultMaterial;
				bindMaterial( material, m_DeferredData.GBufferShader );
				lastMaterial = drawCall.Material;
			}

			m_DeferredData.GBufferShader->SetMatrix4( "u_PVM", m_SceneInfo.ViewProjectionMatrix * drawCall.Transform );
			m_DeferredData.GBufferShader->SetMatrix4( "u_Model", drawCall.Transform );

			drawCall.VAO->Bind();
			RenderCommand::DrawIndexed( drawCall.VAO );
			drawCall.VAO->Unbind();
		}

		m_DeferredData.GBufferShader->Unbind();

	}

	void SceneRenderer::DeferredLightingPass()
	{
		m_DeferredData.LightingShader->Bind();

		// Bind Shader Uniforms
		{
			// - Bind Scene Info -
			m_DeferredData.LightingShader->SetFloat3( "u_CameraPosition", m_SceneInfo.CameraPosition );

			// Incremented every time a texture is bound
			uint32_t textureSlot = 0;

			// - Bind GBuffer Textures -
			{
				m_DeferredData.GBuffer.GetFramebuffer()->BindAttatchment( m_DeferredData.GBuffer.GetPositionAttachment(), 0 );
				m_DeferredData.GBuffer.GetFramebuffer()->BindAttatchment( m_DeferredData.GBuffer.GetNormalAttachment(), 1 );
				m_DeferredData.GBuffer.GetFramebuffer()->BindAttatchment( m_DeferredData.GBuffer.GetAlbedoAttachment(), 2 );
				m_DeferredData.GBuffer.GetFramebuffer()->BindAttatchment( m_DeferredData.GBuffer.GetAOMRAttachment(), 3 );
				m_DeferredData.GBuffer.GetFramebuffer()->BindAttatchment( m_DeferredData.GBuffer.GetEmissionAttachment(), 4 );

				m_DeferredData.LightingShader->SetInt( "g_Position", 0 );
				m_DeferredData.LightingShader->SetInt( "g_Normal", 1 );
				m_DeferredData.LightingShader->SetInt( "g_Albedo", 2 );
				m_DeferredData.LightingShader->SetInt( "g_AORM", 3 );
				m_DeferredData.LightingShader->SetInt( "g_Emission", 4 );

				textureSlot = 5;
			}

			// - Bind Shadow Maps -
			{
				// Directional Lights
				for ( uint32_t i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i )
				{
					DirectionalLight& directionalLight = m_LightEnvironment.DirectionalLights[i];

					if ( directionalLight.ShadowMap )
					{
						directionalLight.ShadowMap->BindDepthAttatchment( textureSlot );
						m_DeferredData.LightingShader->SetInt( ( "u_DirectionalShadowMaps[" + std::to_string( i ) + "]" ).c_str(), textureSlot );
						m_DeferredData.LightingShader->SetMatrix4( "u_LightSpaceMatrix", m_LightViewProjectionMatrix );
					}

					textureSlot++;
				}
			}

			// - Bind Environment Map -
			{
				// Bind BRDF LUT
				if ( m_BrdfLUT )
				{
					m_BrdfLUT->Bind( textureSlot );
					m_DeferredData.LightingShader->SetInt( "u_Environment.BrdfLUT", textureSlot );
				}

				// Bind Environment Map
				if ( m_SceneEnvironment.HDRI.EnvironmentMap && m_SceneEnvironment.HDRI.EnvironmentMap->GetIrradianceMap() )
				{
					if ( auto radianceMap = m_SceneEnvironment.HDRI.EnvironmentMap->GetRadianceMap() )
					{
						radianceMap->Bind( textureSlot );
						m_DeferredData.LightingShader->SetInt( "u_Environment.PrefilterMap", textureSlot );
						textureSlot++;
					}
				}
				m_DeferredData.LightingShader->SetFloat( "u_Environment.Roughness", m_SceneEnvironment.HDRI.Blur );
				m_DeferredData.LightingShader->SetFloat( "u_Environment.Exposure", m_SceneEnvironment.HDRI.Exposure );
				m_DeferredData.LightingShader->SetFloat( "u_Environment.Gamma", m_SceneEnvironment.HDRI.Gamma );
				m_DeferredData.LightingShader->SetFloat( "u_Environment.Intensity", m_SceneEnvironment.HDRI.Intensity );
			}

			// - Bind Lights -
			{
				// Directional Lights
				for ( uint32_t i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i )
				{
					DirectionalLight& directionalLight = m_LightEnvironment.DirectionalLights[i];
					std::string uniformName = "u_DirectionalLights[" + std::to_string( i ) + "].";
					m_DeferredData.LightingShader->SetFloat3( ( uniformName + "Direction" ).c_str(), directionalLight.Direction );
					m_DeferredData.LightingShader->SetFloat3( ( uniformName + "Color" ).c_str(), directionalLight.Color );
					m_DeferredData.LightingShader->SetFloat( ( uniformName + "Intensity" ).c_str(), directionalLight.Intensity );
				}

				// Point Lights
				for ( uint32_t i = 0; i < MAX_POINT_LIGHTS; i++ )
				{
					PointLight& pointLight = m_LightEnvironment.PointLights[i];
					std::string uniformName = "u_PointLights[" + std::to_string( i ) + "].";
					m_DeferredData.LightingShader->SetFloat3( ( uniformName + "Position" ).c_str(), pointLight.Position );
					m_DeferredData.LightingShader->SetFloat3( ( uniformName + "Color" ).c_str(), pointLight.Color );
					m_DeferredData.LightingShader->SetFloat( ( uniformName + "Intensity" ).c_str(), pointLight.Intensity );
					m_DeferredData.LightingShader->SetFloat( ( uniformName + "FalloffExponent" ).c_str(), pointLight.FalloffExponent );
					m_DeferredData.LightingShader->SetFloat( ( uniformName + "AttenuationRadius" ).c_str(), pointLight.AttenuationRadius );
				}

				// Spot Lights
				for ( uint32_t i = 0; i < MAX_SPOT_LIGHTS; ++i )
				{
					SpotLight& spotLight = m_LightEnvironment.SpotLights[i];
					std::string uniformName = "u_SpotLights[" + std::to_string( i ) + "].";
					m_DeferredData.LightingShader->SetFloat3( ( uniformName + "Position" ).c_str(), spotLight.Position );
					m_DeferredData.LightingShader->SetFloat3( ( uniformName + "Direction" ).c_str(), spotLight.Direction );
					m_DeferredData.LightingShader->SetFloat3( ( uniformName + "Color" ).c_str(), spotLight.Color );
					m_DeferredData.LightingShader->SetFloat( ( uniformName + "Intensity" ).c_str(), spotLight.Intensity );
					m_DeferredData.LightingShader->SetFloat( ( uniformName + "FalloffExponent" ).c_str(), spotLight.FalloffExponent );
					m_DeferredData.LightingShader->SetFloat( ( uniformName + "AttenuationRadius" ).c_str(), spotLight.AttenuationRadius );
					m_DeferredData.LightingShader->SetFloat( ( uniformName + "InnerConeAngle" ).c_str(), spotLight.InnerConeAngle );
					m_DeferredData.LightingShader->SetFloat( ( uniformName + "OuterConeAngle" ).c_str(), spotLight.OuterConeAngle );
				}
			}
		}

		// Draw Quad
		{
			RenderCommand::SetDepthTest( false );
			RenderCommand::SetCullMode( ECullMode::None );

			m_DeferredData.QuadVAO->Bind();
			RenderCommand::DrawIndexed( m_DeferredData.QuadVAO );
			m_DeferredData.QuadVAO->Unbind();
		}

		m_DeferredData.LightingShader->Unbind();
	}

	//////////////////////////////////////////////////////////////////////////
	// Forward Rendering
	//////////////////////////////////////////////////////////////////////////

	void SceneRenderer::ForwardRenderPass()
	{
		m_RenderTarget->Bind();
		RenderCommand::Clear();

		// - Reset Viewport -
		const iVector2 viewportSize = m_SceneInfo.Camera.GetViewportSize();
		RenderCommand::SetViewport( 0, 0, viewportSize.x, viewportSize.y );

		// - Geometry Pass -
		{
			ForwardGeometryPass();
		}

		// - Draw Skybox -
		{
			RenderSkybox();
		}

		// - Apply Post-Processing Pass -
		{
			PostProcessPass();
		}

		m_RenderTarget->Unbind();
	}

	void SceneRenderer::ForwardGeometryPass()
	{
		RenderCommand::SetDepthTest( true );
		RenderCommand::SetCullMode( ECullMode::Back );
		RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );

		ShaderHandle lastShader = -1;
		MaterialHandle lastMaterial = -1;
		SharedPtr<Shader> shader = m_DefaultShader;
		SharedPtr<Material> material = m_DefaultMaterial;

		const auto bindShaderUniforms = [this, &shader, &material]()
			{
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

				// Bind environment map
				if ( m_SceneEnvironment.HDRI.EnvironmentMap && m_SceneEnvironment.HDRI.EnvironmentMap->GetIrradianceMap() )
				{
					if ( auto radianceMap = m_SceneEnvironment.HDRI.EnvironmentMap->GetRadianceMap() )
					{
						radianceMap->Bind( 0 );
						shader->SetInt( "u_Environment.PrefilterMap", 0 );
					}
				}
				shader->SetFloat( "u_Environment.Roughness", m_SceneEnvironment.HDRI.Blur );
				shader->SetFloat( "u_Environment.Exposure", m_SceneEnvironment.HDRI.Exposure );
				shader->SetFloat( "u_Environment.Gamma", m_SceneEnvironment.HDRI.Gamma );
				shader->SetFloat( "u_Environment.Intensity", m_SceneEnvironment.HDRI.Intensity );

				// Bind BRDF LUT
				if ( m_BrdfLUT )
				{
					m_BrdfLUT->Bind( 1 );
					shader->SetInt( "u_Environment.BrdfLUT", 1 );
				}

				// Temp ?
				// Bind Shadow Maps
				{
					uint32_t textureSlot = 2;
					for ( uint32_t i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i )
					{
						DirectionalLight& directionalLight = m_LightEnvironment.DirectionalLights[i];

						if ( directionalLight.ShadowMap )
							directionalLight.ShadowMap->BindDepthAttatchment( textureSlot );
						else
							m_WhiteTexture->Bind( textureSlot );
						shader->SetInt( ( "u_DirectionalShadowMaps[" + std::to_string( i ) + "]" ).c_str(), textureSlot );
						shader->SetMatrix4( "u_LightSpaceMatrix", m_LightViewProjectionMatrix );
						textureSlot++;
					}
				}
			};

		const auto bindMaterialUniforms = [this, &material, &shader]()
			{
				// Bind Textures
				uint32_t textureSlot = 2 + MAX_DIRECTIONAL_LIGHTS;
				auto albedoTexture = AssetManager::GetAsset<Texture>( material->AlbedoTexture );
				if ( !albedoTexture ) albedoTexture = m_WhiteTexture;
				albedoTexture->Bind( textureSlot );
				shader->SetInt( "u_AlbedoTexture", textureSlot );

				textureSlot++;
				auto metallicTexture = AssetManager::GetAsset<Texture>( material->MetallicTexture );
				if ( !metallicTexture ) metallicTexture = m_BlackTexture;
				metallicTexture->Bind( textureSlot );
				shader->SetInt( "u_MetallicTexture", textureSlot );

				textureSlot++;
				auto roughnessTexture = AssetManager::GetAsset<Texture>( material->RoughnessTexture );
				if ( !roughnessTexture ) roughnessTexture = m_WhiteTexture;
				roughnessTexture->Bind( textureSlot );
				shader->SetInt( "u_RoughnessTexture", textureSlot );

				textureSlot++;
				auto normalTexture = AssetManager::GetAsset<Texture>( material->NormalTexture );
				if ( !normalTexture ) normalTexture = m_NormalTexture;
				normalTexture->Bind( textureSlot );
				shader->SetInt( "u_NormalTexture", textureSlot );

				textureSlot++;
				TODO( "Should we be setting the opacity texture to the albedo if there isn't an opacity texture?" );
				auto opacityTexture = AssetManager::GetAsset<Texture>( material->OpacityTexture );
				if ( !opacityTexture ) opacityTexture = albedoTexture;
				opacityTexture->Bind( textureSlot );
				shader->SetInt( "u_OpacityTexture", textureSlot );

				textureSlot++;
				auto emissiveTexture = AssetManager::GetAsset<Texture>( material->EmissiveTexture );
				if ( !emissiveTexture ) emissiveTexture = m_BlackTexture;
				emissiveTexture->Bind( textureSlot );
				shader->SetInt( "u_EmissiveTexture", textureSlot );

				textureSlot++;
				auto aoTexture = AssetManager::GetAsset<Texture>( material->AOTexture );
				if ( !aoTexture ) aoTexture = m_WhiteTexture;
				aoTexture->Bind( textureSlot );
				shader->SetInt( "u_AOTexture", textureSlot );
			};

		for ( const auto& drawCall : m_DrawCalls )
		{
			bool shaderChanged = false;
			bool materialChanged = lastMaterial != drawCall.Material;
			if ( materialChanged )
			{
				lastMaterial = drawCall.Material;
				material = AssetManager::GetAsset<Material>( drawCall.Material );
				if ( !material )
					material = m_DefaultMaterial;

				if ( lastShader != material->Shader )
				{
					shader = AssetManager::GetAsset<Shader>( material->Shader );
					if ( !shader )
						shader = m_DefaultShader;

					lastShader = material->Shader;
					shaderChanged = true;
				}
			}

			if ( shaderChanged )
			{
				shader->Bind();
				bindShaderUniforms();
			}

			if ( materialChanged )
			{
				bindMaterialUniforms();
			}

			shader->SetMatrix4( "u_PVM", m_SceneInfo.ViewProjectionMatrix * drawCall.Transform );
			shader->SetMatrix4( "u_Model", drawCall.Transform );
			shader->SetFloat3( "u_CameraPosition", m_SceneInfo.CameraPosition );

			drawCall.VAO->Bind();
			RenderCommand::DrawIndexed( drawCall.VAO );
			drawCall.VAO->Unbind();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Skybox
	//////////////////////////////////////////////////////////////////////////

	void SceneRenderer::RenderSkybox()
	{
		if ( m_SceneEnvironment.HDRI.EnvironmentMap && m_SceneEnvironment.HDRI.EnvironmentMap->GetRadianceMap() )
		{
			auto radianceMap = m_SceneEnvironment.HDRI.EnvironmentMap->GetRadianceMap();
			RenderCommand::SetDepthTest( true );
			RenderCommand::SetDepthCompare( EDepthCompareOperator::LessOrEqual );
			RenderCommand::SetCullMode( ECullMode::None );
			m_SkyboxShader->Bind();
			{
				Matrix4 skyboxView = Matrix4( Matrix3( m_SceneInfo.ViewMatrix ) ); // Remove translation
				Matrix4 skyboxTransform = glm::scale( Matrix4( 1.0f ), { 100.0f, 100.0f, 100.0f } );

				m_SkyboxShader->SetMatrix4( "u_Projection", m_SceneInfo.ProjectionMatrix );
				m_SkyboxShader->SetMatrix4( "u_View", skyboxView );
				m_SkyboxShader->SetMatrix4( "u_Model", skyboxTransform );
				m_SkyboxShader->SetFloat( "u_Exposure", m_SceneEnvironment.HDRI.Exposure );
				m_SkyboxShader->SetFloat( "u_Gamma", m_SceneEnvironment.HDRI.Gamma );
				const uint32_t MaxMipLevels = 5;
				float roughness = static_cast<float>( m_SceneEnvironment.HDRI.Blur * ( MaxMipLevels - 1 ) );
				m_SkyboxShader->SetFloat( "u_Roughness", roughness );

				radianceMap->Bind();

				SharedPtr<VertexArray>& cubeVAO = m_CubeMesh->GetSubMeshes()[0].VAO;
				cubeVAO->Bind();
				RenderCommand::DrawIndexed( cubeVAO );
				cubeVAO->Unbind();

				radianceMap->Unbind();
			}
			m_SkyboxShader->Unbind();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Post Processing
	//////////////////////////////////////////////////////////////////////////

	void SceneRenderer::PostProcessPass()
	{
		// - Apply Bloom -
		{

		}

		// - Apply Tone Mapping -
		{

		}

		// - Apply Anti-Aliasing -
		{

		}
	}

}