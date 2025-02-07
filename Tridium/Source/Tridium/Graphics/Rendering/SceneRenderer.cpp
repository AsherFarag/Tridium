#include "tripch.h"
#include "SceneRenderer.h"
#include <Tridium/Scene/Scene.h>
#include <Tridium/Asset/AssetManager.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Graphics/Rendering/Renderer.h>
#include <Tridium/Graphics/Rendering/RenderCommand.h>
#include <Tridium/Graphics/Rendering/Shader.h>
#include <Tridium/Graphics/Rendering/Texture.h>
#include <Tridium/Graphics/Rendering/Material.h>
#include <Tridium/Graphics/Rendering/Mesh.h>
#include <Tridium/Graphics/Rendering/EnvironmentMap.h>
#include <Tridium/Asset/Loaders/TextureLoader.h>

namespace Tridium {

	SceneRenderer::SceneRenderer( Scene& a_Scene )
		: m_Scene( a_Scene ), m_SceneEnvironment( a_Scene.GetSceneEnvironment() )
	{
		Clear();

		// Set Default Assets
		{
			TODO( "Use the asset manager instead." );
			m_DefaultShader.reset( Shader::Create() );
			m_DefaultShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/PBR-Shadows.glsl" );
			m_SkyboxShader.reset( Shader::Create() );
			m_SkyboxShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/EnvironmentMap/SkyBox.glsl" );
			m_WhiteTexture = AssetManager::GetAsset<Texture>( TextureFactory::GetWhiteTexture() );
			m_BlackTexture = AssetManager::GetAsset<Texture>( TextureFactory::GetBlackTexture() );
			m_NormalTexture = AssetManager::GetAsset<Texture>( TextureFactory::GetNormalTexture() );
			m_DefaultMaterial = MakeShared<Material>();
			m_DefaultMaterial->MetallicIntensity = 0.0f;
			m_DefaultMaterial->RoughnessIntensity = 1.0f;
			m_DefaultMaterial->EmissiveIntensity = 0.0f;
			m_DefaultMaterial->AlbedoColor = { 1.0f, 1.0f, 1.0f };
			m_DefaultMaterial->AlbedoTexture = TextureFactory::GetWhiteTexture();
			m_DefaultMaterial->MetallicTexture = TextureFactory::GetWhiteTexture();
			m_DefaultMaterial->RoughnessTexture = TextureFactory::GetWhiteTexture();


			// Load BRDF LUT
			TextureSpecification spec;
			spec.WrapS = ETextureWrap::ClampToEdge;
			spec.WrapT = ETextureWrap::ClampToEdge;
			spec.WrapR = ETextureWrap::ClampToEdge;
			spec.MinFilter = ETextureFilter::Linear;
			spec.MagFilter = ETextureFilter::Linear;
			m_BrdfLUT = TextureLoader::LoadTexture( spec, Application::GetEngineAssetsDirectory() / "Renderer/BRDF-LUT.png" );
			TE_CORE_ASSERT( m_BrdfLUT, "Failed to load BRDF LUT" );
		}

		// Set Debug Assets
		{
			m_DebugSimpleShader.reset( Shader::Create() );
			m_DebugSimpleShader->Compile( Application::GetEngineAssetsDirectory() / "Shaders/Simple.glsl" );

			m_DebugSphereVAO = MeshFactory::CreateSphere()->GetSubMeshes()[0].VAO;
			m_DebugCubeVAO = MeshFactory::CreateCube()->GetSubMeshes()[0].VAO;
			m_DebugCapsuleVAO = MeshFactory::CreateCapsule( 0.5f, 1.0f, 1u )->GetSubMeshes()[0].VAO;
			m_DebugCylinderVAO = MeshFactory::CreateCylinder( 0.5f, 0.5f, 1.0f, 1 )->GetSubMeshes()[0].VAO;
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
		PROFILE_FUNCTION( ProfilerCategory::Rendering );
		// Reset per frame data
		Clear();

		//ScopedTimer renderTime( m_RenderStats.RenderTime );

		m_RenderTarget = a_FBO;

		BeginScene( a_Camera, a_View, a_CameraPosition );

		// - Generate Shadow Maps -
		{
			GenerateShadowMaps();
		}

		// - Render Pass -
		{
			switch ( m_RenderSettings.RenderMode )
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
		}

		m_RenderTarget->Bind();

		// - Draw Skybox -
		{
			RenderSkybox();
		}

		// - Apply Post-Processing Pass -
		{
			//ScopedTimer postProcessTime( m_RenderStats.PostProcessTime );
			PostProcessPass();
		}

		// - Debug Draw Colliders -
		if ( m_RenderSettings.DebugDrawColliders )
		{
			DebugRenderColliders();
		}

		m_RenderTarget->Unbind();

		EndScene();
	}

	void SceneRenderer::BeginScene( const Camera& a_Camera, const Matrix4& a_View, const Vector3& a_CameraPosition )
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		// Extract Camera Forward from the View Matrix
		Vector3 cameraForward = Vector3( a_View[0][2], a_View[1][2], a_View[2][2] );

		m_SceneInfo = SceneInfo
		{
			.ProjectionMatrix = a_Camera.GetProjection(),
			.ViewMatrix = a_View,
			.ViewProjectionMatrix = a_Camera.GetProjection() * a_View,
			.CameraPosition = a_CameraPosition,
			.Camera = a_Camera,
			.CameraFrustum = a_Camera.GetFrustum( a_CameraPosition, cameraForward ),
		};

		//Debug::DrawFrustum( m_SceneInfo.ProjectionMatrix * m_SceneInfo.ViewMatrix, Debug::Colors::Green, Debug::EDrawDuration::OneFrame );

		// - Submit Directional Lights -
		{
			PROFILE_SCOPE( "Submit Directional Lights", ProfilerCategory::Rendering );
			auto directionalLightComponents = m_Scene.GetECS().View<DirectionalLightComponent, TransformComponent>();
			m_LightEnvironment.NumDirectionalLights = 0;
			directionalLightComponents.each(
				[&]( auto go, DirectionalLightComponent& lightComponent, TransformComponent& transform )
				{
					if ( m_LightEnvironment.NumDirectionalLights >= MAX_DIRECTIONAL_LIGHTS )
						return;

					// TEMP!
					if ( lightComponent.CastsShadows && !lightComponent.ShadowMap )
					{
						FramebufferSpecification spec =
						{
							.Width = (uint32_t)lightComponent.ShadowMapSize.x,
							.Height = (uint32_t)lightComponent.ShadowMapSize.y,
							.Attachments = { EFramebufferTextureFormat::Depth },
						};

						lightComponent.ShadowMap = Framebuffer::Create( spec );
					}

					if ( !lightComponent.CastsShadows )
					{
						lightComponent.ShadowMap.reset();
					}

					DirectionalLight& light = m_LightEnvironment.DirectionalLights[m_LightEnvironment.NumDirectionalLights];

					light.Direction = transform.GetForward();
					light.Color = lightComponent.LightColor;
					light.Intensity = lightComponent.Intensity;
					light.CastsShadows = lightComponent.CastsShadows;
					light.ShadowMapSize = lightComponent.ShadowMapSize;
					light.ShadowMap = lightComponent.ShadowMap;

					// - Update View Projection Matrix -
					float lightNearPlane = -1000.0f; float lightFarPlane = 1000.0f;
					Matrix4 lightView = glm::lookAt(
						m_SceneInfo.CameraPosition + ( light.Direction * -20.f ),
						m_SceneInfo.CameraPosition,
						{ 0.0f, 1.0f, 0.0f } );
					float size = 100.0f;
					Matrix4 lightProjection = glm::ortho( -size, size, -size, size, lightNearPlane, lightFarPlane );
					light.LightSpaceMatrix = lightProjection * lightView;

					m_LightEnvironment.NumDirectionalLights++;
				}
			);
		}

		// - Submit Point Lights -
		{
			PROFILE_SCOPE( "Submit Point Lights", ProfilerCategory::Rendering );
			auto pointLightComponents = m_Scene.GetECS().View<PointLightComponent, TransformComponent>();
			m_LightEnvironment.NumPointLights = 0;
			pointLightComponents.each(
				[&]( auto go, PointLightComponent& lightComponent, TransformComponent& transform )
				{
					if ( m_LightEnvironment.NumPointLights >= MAX_POINT_LIGHTS )
						return;

					// TEMP!
					if ( lightComponent.CastsShadows && !lightComponent.ShadowMap )
					{
						FramebufferTextureSpecification cubeMapSpec( EFramebufferTextureFormat::Depth, EFramebufferTextureType::TextureCubeMap );

						FramebufferSpecification spec =
						{
							.Width = (uint32_t)lightComponent.ShadowMapSize,
							.Height = (uint32_t)lightComponent.ShadowMapSize,
							.Attachments = { cubeMapSpec }
						};

						lightComponent.ShadowMap = Framebuffer::Create( spec );
					}

					if ( !lightComponent.CastsShadows )
					{
						lightComponent.ShadowMap.reset();
					}

					PointLight& light = m_LightEnvironment.PointLights[m_LightEnvironment.NumPointLights];

					light.Position = transform.GetWorldPosition();
					light.Color = lightComponent.LightColor;
					light.Intensity = lightComponent.Intensity;
					light.FalloffExponent = lightComponent.FalloffExponent;
					light.AttenuationRadius = lightComponent.AttenuationRadius;
					light.CastsShadows = lightComponent.CastsShadows;
					light.ShadowMapSize = lightComponent.ShadowMapSize;
					light.ShadowMap = lightComponent.ShadowMap;

					Matrix4 shadowProjection = glm::perspective( glm::radians( 90.0f ), 1.0f, 0.1f, light.AttenuationRadius );
					light.LightSpaceMatrices[0] = shadowProjection * glm::lookAt( light.Position, light.Position + Vector3( 1.0f, 0.0f, 0.0f ), Vector3( 0.0f, -1.0f, 0.0f ) );
					light.LightSpaceMatrices[1] = shadowProjection * glm::lookAt( light.Position, light.Position + Vector3( -1.0f, 0.0f, 0.0f ), Vector3( 0.0f, -1.0f, 0.0f ) );
					light.LightSpaceMatrices[2] = shadowProjection * glm::lookAt( light.Position, light.Position + Vector3( 0.0f, 1.0f, 0.0f ), Vector3( 0.0f, 0.0f, 1.0f ) );
					light.LightSpaceMatrices[3] = shadowProjection * glm::lookAt( light.Position, light.Position + Vector3( 0.0f, -1.0f, 0.0f ), Vector3( 0.0f, 0.0f, -1.0f ) );
					light.LightSpaceMatrices[4] = shadowProjection * glm::lookAt( light.Position, light.Position + Vector3( 0.0f, 0.0f, 1.0f ), Vector3( 0.0f, -1.0f, 0.0f ) );
					light.LightSpaceMatrices[5] = shadowProjection * glm::lookAt( light.Position, light.Position + Vector3( 0.0f, 0.0f, -1.0f ), Vector3( 0.0f, -1.0f, 0.0f ) );

					m_LightEnvironment.NumPointLights++;
				}
			);
		}

		// - Submit Spot Lights -
		{
			PROFILE_SCOPE( "Submit Spot Lights", ProfilerCategory::Rendering );
			auto spotLightComponents = m_Scene.GetECS().View<SpotLightComponent, TransformComponent>();
			m_LightEnvironment.NumSpotLights = 0;
			spotLightComponents.each(
				[&]( auto go, SpotLightComponent& lightComponent, TransformComponent& transform )
				{
					if ( m_LightEnvironment.NumSpotLights >= MAX_SPOT_LIGHTS )
						return;

					// TEMP!
					if ( lightComponent.CastsShadows && !lightComponent.ShadowMap )
					{
						FramebufferSpecification spec =
						{
							.Width = (uint32_t)lightComponent.ShadowMapSize.x,
							.Height = (uint32_t)lightComponent.ShadowMapSize.y,
							.Attachments = { EFramebufferTextureFormat::Depth },
						};

						lightComponent.ShadowMap = Framebuffer::Create( spec );
					}

					SpotLight& light = m_LightEnvironment.SpotLights[m_LightEnvironment.NumSpotLights];

					light.Position = transform.GetWorldPosition();
					light.Direction = transform.GetForward();
					light.Color = lightComponent.LightColor;
					light.Intensity = lightComponent.Intensity;
					light.FalloffExponent = lightComponent.FalloffExponent;
					light.AttenuationRadius = lightComponent.AttenuationRadius;
					light.InnerConeAngle = lightComponent.InnerConeAngle;
					light.OuterConeAngle = lightComponent.OuterConeAngle;
					light.CastsShadows = lightComponent.CastsShadows;
					light.ShadowMapSize = lightComponent.ShadowMapSize;
					light.ShadowMap = lightComponent.ShadowMap;

					float nearPlane = 0.1f; float farPlane = 1000.0f;
					Matrix4 lightView = glm::lookAt( light.Position, light.Position + light.Direction, { 0.0f, 1.0f, 0.0f } );
					Matrix4 lightProjection = glm::perspective( glm::radians( light.OuterConeAngle ) * 2.0f, 1.0f, nearPlane, farPlane );
					light.LightSpaceMatrix = lightProjection * lightView;

					m_LightEnvironment.NumSpotLights++;
				}
			);
		}

		// - Submit Static Mesh Components to the Draw List -
		{
			PROFILE_SCOPE( "Submit Static Mesh Components", ProfilerCategory::Rendering );
			auto meshComponents = m_Scene.GetECS().View<StaticMeshComponent, TransformComponent>();
			meshComponents.each( [&]( auto go, StaticMeshComponent& meshComponent, TransformComponent& transform )
				{
					DrawPass passFlags = EDrawPass::Opaque;
					if ( meshComponent.CastShadows ) passFlags |= EDrawPass::Shadows;

					// Do a Fustrum Cull check:
					//	- If the mesh is not in the camera's view frustum, skip it
					//	- If the mesh is in the camera's view frustum, add it to the draw list
					{
						// Get the mesh bounds
						SharedPtr<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>( meshComponent.Mesh );
						if ( !mesh )
							return;

						// Get the mesh bounds
						AABB meshBounds = mesh->GetBoundingBox().Transform( transform.GetWorldTransform() );

						//Debug::DrawAABB( meshBounds, Debug::Colors::Red, Debug::EDrawDuration::OneFrame);

						// Check if the mesh is in the camera's view frustum
						//if ( !m_SceneInfo.CameraFrustum.Intersects( meshBounds ) )
						//{
						//	m_RenderStats.CulledDrawCalls++;
						//	return;
						//}
					}


					m_DrawList.AddCommand( passFlags, meshComponent.Mesh, meshComponent.Materials, transform.GetWorldTransform() );
				}
			);
		}
	}

	void SceneRenderer::EndScene()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		// Release ownership of the render target
		m_RenderTarget.reset();
	}

	void SceneRenderer::Clear()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		m_DrawList.Clear();

		for ( auto& pointLight : m_LightEnvironment.PointLights )
			pointLight = {};

		for ( auto& spotLight : m_LightEnvironment.SpotLights )
			spotLight = {};

		for ( auto& directionalLight : m_LightEnvironment.DirectionalLights )
			directionalLight = {};

		// Reset Render Stats
		m_RenderStats = {};
	}

	//////////////////////////////////////////////////////////////////////////
	// Shadows
	//////////////////////////////////////////////////////////////////////////

	void SceneRenderer::GenerateShadowMaps()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		RenderCommand::SetDepthTest( true );

		TODO( "Front face culling only works on opaque objects, need to handle transparent objects by using a bias." );
		// Set Cull Mode to front to avoid peter panning
		RenderCommand::SetCullMode( ECullMode::Front );
		RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );

		// Generate Directional Light Shadow Maps
		{
			m_ShadowMapShader->Bind();

			for ( auto& directionalLight : m_LightEnvironment.DirectionalLights )
			{
				if ( !directionalLight.CastsShadows )
					continue;

				RenderCommand::SetViewport( 0, 0, directionalLight.ShadowMapSize.x, directionalLight.ShadowMapSize.y );
				directionalLight.ShadowMap->Resize( directionalLight.ShadowMapSize.x, directionalLight.ShadowMapSize.y );
				directionalLight.ShadowMap->Bind();
				RenderCommand::Clear();

				const auto& drawCommands = m_DrawList.GetCommandListByPass( EDrawPass::Shadows );
				for ( const auto& [key, drawCall] : drawCommands )
				{
					drawCall.VAO->Bind();
					for ( const Matrix4& transform : drawCall.Transforms )
					{
						m_ShadowMapShader->SetMatrix4( "u_PVM", directionalLight.LightSpaceMatrix * transform );
						RenderCommand::DrawIndexed( drawCall.VAO );
					}
					drawCall.VAO->Unbind();
				}

				directionalLight.ShadowMap->Unbind();
			}

			m_ShadowMapShader->Unbind();
		}

		// Generate Point Light Shadow Maps
		{
			m_ShadowCubeMapShader->Bind();

			for ( PointLight& pointLight : m_LightEnvironment.PointLights )
			{
				if ( !pointLight.CastsShadows )
					continue;

				// Bind Uniforms
				{
					for ( uint32_t i = 0; i < 6; i++ )
					{
						const std::string uniformName = "u_LightSpaceMatrices[" + std::to_string( i ) + "]";
						m_ShadowCubeMapShader->SetMatrix4( uniformName.c_str(), pointLight.LightSpaceMatrices[i] );
					}

					m_ShadowCubeMapShader->SetFloat( "u_FarPlane", pointLight.AttenuationRadius );
					m_ShadowCubeMapShader->SetFloat3( "u_LightPosition", pointLight.Position );
				}

				RenderCommand::SetViewport( 0, 0, pointLight.ShadowMapSize, pointLight.ShadowMapSize );
				pointLight.ShadowMap->Resize( pointLight.ShadowMapSize, pointLight.ShadowMapSize );
				pointLight.ShadowMap->Bind();
				RenderCommand::Clear();

				const auto& drawCommands = m_DrawList.GetCommandListByPass( EDrawPass::Shadows );
				for ( const auto& [key, drawCall] : drawCommands )
				{
					drawCall.VAO->Bind();
					for ( const Matrix4& transform : drawCall.Transforms )
					{
						m_ShadowCubeMapShader->SetMatrix4( "u_Model", transform );
						RenderCommand::DrawIndexed( drawCall.VAO );
					}
					drawCall.VAO->Unbind();
				}

				pointLight.ShadowMap->Unbind();
			}

			m_ShadowCubeMapShader->Unbind();
		}

		// Generate Spot Light Shadow maps
		{
			m_ShadowMapShader->Bind();

			for ( auto& spotLight : m_LightEnvironment.SpotLights )
			{
				if ( !spotLight.CastsShadows )
					continue;

				RenderCommand::SetViewport( 0, 0, spotLight.ShadowMapSize.x, spotLight.ShadowMapSize.y );
				spotLight.ShadowMap->Resize( spotLight.ShadowMapSize.x, spotLight.ShadowMapSize.y );
				spotLight.ShadowMap->Bind();
				RenderCommand::Clear();

				const auto& drawCommands = m_DrawList.GetCommandListByPass( EDrawPass::Shadows );
				for ( const auto& [key, drawCall] : drawCommands )
				{
					drawCall.VAO->Bind();
					for ( const Matrix4& transform : drawCall.Transforms )
					{
						m_ShadowMapShader->SetMatrix4( "u_PVM", spotLight.LightSpaceMatrix * transform );
						RenderCommand::DrawIndexed( drawCall.VAO );
					}
					drawCall.VAO->Unbind();
				}

				spotLight.ShadowMap->Unbind();
			}

			m_ShadowMapShader->Unbind();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Deferred Rendering
	//////////////////////////////////////////////////////////////////////////

	void SceneRenderer::DeferredRenderPass()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		Vector2 viewportSize = m_SceneInfo.Camera.GetViewportSize();
		viewportSize *= m_RenderSettings.RenderScale;

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

		m_RenderTarget->Unbind();
	}

	void SceneRenderer::DeferredGBufferPass()
	{
		RenderCommand::SetDepthTest( true );
		RenderCommand::SetCullMode( ECullMode::Back );
		RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );

		m_DeferredData.GBufferShader->Bind();
		MaterialHandle lastMaterial = MaterialHandle::InvalidID;

		auto bindMaterial = [&]( SharedPtr<Material>& material, SharedPtr<Shader>& shader )
			{
				// Bind Material Properties
				shader->SetFloat3( "u_AlbedoColor", material->AlbedoColor );
				shader->SetFloat( "u_MetallicIntensity", material->MetallicIntensity );
				shader->SetFloat( "u_RoughnessIntensity", material->RoughnessIntensity );
				shader->SetFloat( "u_EmissiveIntensity", material->EmissiveIntensity );

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

		const auto& drawCommands = m_DrawList.GetCommandListByPass( EDrawPass::Opaque );
		for ( const auto& [key, drawCall] : drawCommands )
		{
			// Bind Material
			if ( lastMaterial != key.Material )
			{
				lastMaterial = key.Material;
				SharedPtr<Material> mat = AssetManager::GetAsset<Material>( key.Material );
				if ( !mat )
				{
					mat = m_DefaultMaterial;
					lastMaterial = MaterialHandle::InvalidID;
				}

				bindMaterial( mat, m_DeferredData.GBufferShader );
			}

			drawCall.VAO->Bind();
			for ( const Matrix4& transform : drawCall.Transforms )
			{
				m_DeferredData.GBufferShader->SetMatrix4( "u_PVM", m_SceneInfo.ViewProjectionMatrix * transform );
				m_DeferredData.GBufferShader->SetMatrix4( "u_Model", transform );
				DrawCall( drawCall.VAO );
			}
			drawCall.VAO->Unbind();
		}

		m_DeferredData.GBufferShader->Unbind();

	}

	void SceneRenderer::DeferredLightingPass()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

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
				m_DeferredData.GBuffer.GetFramebuffer()->BindAttatchment( m_DeferredData.GBuffer.GetOMRAttachment(), 3 );
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
				for ( uint32_t i = 0; i < m_LightEnvironment.NumDirectionalLights; ++i )
				{
					DirectionalLight& directionalLight = m_LightEnvironment.DirectionalLights[i];

					if ( directionalLight.ShadowMap )
					{
						directionalLight.ShadowMap->BindDepthAttatchment( textureSlot );
						m_DeferredData.LightingShader->SetInt( ( "u_DirectionalShadowMaps[" + std::to_string( i ) + "]" ).c_str(), textureSlot );
					}
					else
					{
						m_WhiteTexture->Bind( textureSlot );
						m_DeferredData.LightingShader->SetInt( ( "u_DirectionalShadowMaps[" + std::to_string( i ) + "]" ).c_str(), textureSlot );
					}

					textureSlot++;
				}

				// Point Lights
				for ( uint32_t i = 0; i < m_LightEnvironment.NumPointLights; i++ )
				{
					PointLight& pointLight = m_LightEnvironment.PointLights[i];

					if ( pointLight.ShadowMap )
					{
						pointLight.ShadowMap->BindDepthAttatchment( textureSlot );
						m_DeferredData.LightingShader->SetInt( ( "u_PointShadowMaps[" + std::to_string( i ) + "]" ).c_str(), textureSlot );
					}
					else
					{
						m_WhiteTexture->Bind( textureSlot );
						m_DeferredData.LightingShader->SetInt( ( "u_PointShadowMaps[" + std::to_string( i ) + "]" ).c_str(), textureSlot );
					}

					textureSlot++;
				}

				// Spot Lights
				for ( uint32_t i = 0; i < m_LightEnvironment.NumSpotLights; ++i )
				{
					SpotLight& spotLight = m_LightEnvironment.SpotLights[i];

					if ( spotLight.ShadowMap )
					{
						spotLight.ShadowMap->BindDepthAttatchment( textureSlot );
						m_DeferredData.LightingShader->SetInt( ( "u_SpotShadowMaps[" + std::to_string( i ) + "]" ).c_str(), textureSlot );
					}
					else
					{
						m_WhiteTexture->Bind( textureSlot );
						m_DeferredData.LightingShader->SetInt( ( "u_SpotShadowMaps[" + std::to_string( i ) + "]" ).c_str(), textureSlot );
					}

					textureSlot++;
				}
			}

			// - Bind Environment Map -
			{
				// Bind BRDF LUT
				//if ( m_BrdfLUT )
				//{
				//	m_BrdfLUT->Bind( textureSlot );
				//	m_DeferredData.LightingShader->SetInt( "u_Environment.BrdfLUT", textureSlot );
				//}

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
				m_DeferredData.LightingShader->SetInt( "u_NumDirectionalLights", m_LightEnvironment.NumDirectionalLights );
				for ( uint32_t i = 0; i < m_LightEnvironment.NumDirectionalLights; ++i )
				{
					DirectionalLight& directionalLight = m_LightEnvironment.DirectionalLights[i];
					std::string uniformName = "u_DirectionalLights[" + std::to_string( i ) + "].";
					m_DeferredData.LightingShader->SetFloat3( ( uniformName + "Direction" ).c_str(), directionalLight.Direction );
					m_DeferredData.LightingShader->SetFloat3( ( uniformName + "Color" ).c_str(), directionalLight.Color );
					m_DeferredData.LightingShader->SetFloat( ( uniformName + "Intensity" ).c_str(), directionalLight.Intensity );
					m_DeferredData.LightingShader->SetMatrix4( ( uniformName + "LightSpaceMatrix" ).c_str(), directionalLight.LightSpaceMatrix );
				}

				// Point Lights
				m_DeferredData.LightingShader->SetInt( "u_NumPointLights", m_LightEnvironment.NumPointLights );
				for ( uint32_t i = 0; i < m_LightEnvironment.NumPointLights; i++ )
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
				m_DeferredData.LightingShader->SetInt( "u_NumSpotLights", m_LightEnvironment.NumSpotLights );
				for ( uint32_t i = 0; i < m_LightEnvironment.NumSpotLights; ++i )
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
					m_DeferredData.LightingShader->SetMatrix4( ( uniformName + "LightSpaceMatrix" ).c_str(), spotLight.LightSpaceMatrix );
				}
			}
		}

		// Draw Quad
		{
			RenderCommand::SetDepthTest( false );
			RenderCommand::SetCullMode( ECullMode::None );

			m_DeferredData.QuadVAO->Bind();
			DrawCall( m_DeferredData.QuadVAO );
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
				//{
				//	uint32_t textureSlot = 2;
				//	for ( uint32_t i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i )
				//	{
				//		DirectionalLight& directionalLight = m_LightEnvironment.DirectionalLights[i];

				//		if ( directionalLight.ShadowMap )
				//			directionalLight.ShadowMap->BindDepthAttatchment( textureSlot );
				//		else
				//			m_WhiteTexture->Bind( textureSlot );
				//		shader->SetInt( ( "u_DirectionalShadowMaps[" + std::to_string( i ) + "]" ).c_str(), textureSlot );
				//		shader->SetMatrix4( "u_LightSpaceMatrix", m_LightViewProjectionMatrix );
				//		textureSlot++;
				//	}
				//}
			};

		const auto bindMaterialUniforms = [this, &material, &shader]()
			{
				// Bind Material Properties
				shader->SetFloat3( "u_AlbedoColor", material->AlbedoColor );
				shader->SetFloat( "u_MetallicIntensity", material->MetallicIntensity );
				shader->SetFloat( "u_RoughnessIntensity", material->RoughnessIntensity );
				shader->SetFloat( "u_EmissiveIntensity", material->EmissiveIntensity );

				// Bind Textures
				uint32_t textureSlot = 2 + MAX_DIRECTIONAL_LIGHTS;
				auto albedoTexture = AssetManager::GetAsset<Texture>( material->AlbedoTexture );
				if ( !albedoTexture ) albedoTexture = m_WhiteTexture;
				albedoTexture->Bind( textureSlot );
				shader->SetInt( "u_AlbedoTexture", textureSlot );

				textureSlot++;
				auto metallicTexture = AssetManager::GetAsset<Texture>( material->MetallicTexture );
				if ( !metallicTexture ) metallicTexture = m_WhiteTexture;
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

		//for ( const auto& drawCall : m_DrawCalls )
		//{
		//	bool shaderChanged = false;
		//	bool materialChanged = lastMaterial != drawCall.Material;
		//	if ( materialChanged )
		//	{
		//		lastMaterial = drawCall.Material;
		//		material = AssetManager::GetAsset<Material>( drawCall.Material );
		//		if ( !material )
		//			material = m_DefaultMaterial;

		//		if ( lastShader != material->Shader )
		//		{
		//			shader = AssetManager::GetAsset<Shader>( material->Shader );
		//			if ( !shader )
		//				shader = m_DefaultShader;

		//			lastShader = material->Shader;
		//			shaderChanged = true;
		//		}
		//	}

		//	if ( shaderChanged )
		//	{
		//		shader->Bind();
		//		bindShaderUniforms();
		//	}

		//	if ( materialChanged )
		//	{
		//		bindMaterialUniforms();
		//	}

		//	shader->SetMatrix4( "u_PVM", m_SceneInfo.ViewProjectionMatrix * drawCall.Transform );
		//	shader->SetMatrix4( "u_Model", drawCall.Transform );
		//	shader->SetFloat3( "u_CameraPosition", m_SceneInfo.CameraPosition );

		//	drawCall.VAO->Bind();
		//	RenderCommand::DrawIndexed( drawCall.VAO );
		//	drawCall.VAO->Unbind();
		//}
	}

	//////////////////////////////////////////////////////////////////////////
	// Skybox
	//////////////////////////////////////////////////////////////////////////

	void SceneRenderer::RenderSkybox()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

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
				DrawCall( cubeVAO );
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
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

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

	void SceneRenderer::DebugRenderColliders()
	{
		PROFILE_FUNCTION( ProfilerCategory::Rendering );

		const Vector4 ColliderColor = { 0.72f, 0.85f, 0.65f, 1.0f };

	#if USE_DEBUG_RENDERER

		m_Scene.GetPhysicsScene()->RenderDebug( m_SceneInfo.ViewProjectionMatrix );

	#endif
	}

	void SceneRenderer::DrawCall( const SharedPtr<VertexArray>& a_VAO )
	{
		m_RenderStats.NumDrawCalls++;
		RenderCommand::DrawIndexed( a_VAO );
	}

}