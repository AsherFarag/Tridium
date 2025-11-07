#include "RendererSceneSystem.h"
#include <Tridium/Asset/AssetDatabase.h>
#include <Tridium/Graphics/Renderer/RendererComponents.h>
#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Graphics/RHI/RHI.h>

namespace Tridium {

	void RendererSceneSystem::OnUpdate( float a_DeltaTime )
	{
		RenderPipelineManager* renderPipeline = RendererModule::GetPipelineManager();

		// Set up lighting environment
		{
			LightEnvironment lightEnv;

			// Set up skybox
			auto skyboxes = OwningScene().Registry().View<SkyboxComponent>();
			for ( Entity entity : skyboxes )
			{
				SkyboxComponent& skybox = skyboxes.Get<SkyboxComponent>( entity );

				if ( const auto& environmentMap = skybox.EnvironmentMap.GetOrLoad() )
				{
					lightEnv.Sky.EnvironmentMap = RenderResourceManager::GetOrCreateEnvironmentMap( environmentMap );
					lightEnv.Sky.Exposure = skybox.Exposure;
					lightEnv.Sky.Gamma = skybox.Gamma;
					lightEnv.Sky.Blur = skybox.Blur;
					lightEnv.Sky.Intensity = skybox.Intensity;
				}

				break; // Only use the first skybox found
			}

			// - Directional Lights
			auto dirLights = OwningScene().Registry().Group<DirectionalLightComponent>( EntityInclude<TransformComponent> );
			for ( auto [entity, dirLightComp, transform] : dirLights.Each() )
			{
				if ( !dirLightComp.Enabled )
					continue;

				if ( lightEnv.DirectionalLights.Size() >= LightEnvironment::MaxDirectionalLights )
					break;

				DirectionalLight dirLight;
				dirLight.Direction = -transform.WorldForward( OwningScene().Registry(), entity ).Normalized();
				// Set to default direction for now
				dirLight.Color = dirLightComp.Color;
				dirLight.Intensity = dirLightComp.Intensity;
				dirLight.SpecularScale = dirLightComp.SpecularScale;

				// Simple orthographic bounds; in a real engine, calculate bounds from camera frustum
				const float orthoSize = 50.0f;
				const float nearPlane = -1000.f;
				const float farPlane = 1000.0f;

				Matrix4 lightView = glm::lookAtRH( glm::vec3(-dirLight.Direction * 50.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
				Matrix4 lightProj = glm::orthoRH_ZO( -orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane );
				lightProj[1][1] *= -1; // Invert Y for Vulkan/DirectX
				dirLight.LightSpaceMatrix = (lightProj * lightView);

				RHITextureRef shadowMap = dirLightComp.CastShadows ? dirLightComp.ShadowMap : nullptr;
				if ( dirLightComp.CastShadows && 
					 ( !shadowMap || shadowMap->Desc().Width != dirLightComp.ShadowMapResolution.X || shadowMap->Desc().Height != dirLightComp.ShadowMapResolution.Y ) )
				{
					auto texDesc = RHITextureDesc{}
						.SetName( "Directional Shadow Map" )
						.SetDimension( ERHITextureDimension::Texture2D )
						.SetWidth( dirLightComp.ShadowMapResolution.X )
						.SetHeight( dirLightComp.ShadowMapResolution.Y )
						.SetMips( 1 )
						.SetFormat( ERHIFormat::D32_FLOAT )
						.SetBindFlags( ERHIBindFlags::DepthStencil | ERHIBindFlags::ShaderResource )
						.SetHeapType( ERHIHeapType::Default )
						.SetUseClearValue( true )
						.SetInitialState( ERHIResourceStates::DepthStencilWrite );

					shadowMap = dirLightComp.ShadowMap = RHI::CreateTexture( texDesc );
				}

				lightEnv.AddDirectionalLight( dirLight, std::move( shadowMap ) );
			}

			// - Point Lights
			OwningScene().Registry().Group<PointLightComponent>( EntityInclude<TransformComponent> ).Each(
			[&]( Entity entity, const PointLightComponent& pointLightComp, const TransformComponent& transform )
			{
				if ( !pointLightComp.Enabled )
					return;

				PointLight pointLight;
				pointLight.Position = transform.WorldPosition( OwningScene().Registry(), entity );
				pointLight.Intensity = pointLightComp.Intensity;
				pointLight.Color = pointLightComp.Color;
				pointLight.MinRadius = pointLightComp.MinRadius;
				pointLight.Radius = pointLightComp.Radius;
				pointLight.Falloff = pointLightComp.Falloff;
				pointLight.SourceSize = pointLightComp.SourceSize;
				pointLight.SpecularScale = pointLightComp.SpecularScale;
				lightEnv.PointLights.PushBack( pointLight );
			} );

			// - Spot Lights
			OwningScene().Registry().Group<SpotLightComponent>( EntityInclude<TransformComponent> ).Each(
			[&]( Entity entity, const SpotLightComponent& spotLightComp, const TransformComponent& transform )
			{
				if ( !spotLightComp.Enabled )
					return;

				Vector3 position;
				Quaternion rotation;
				Vector3 scale;
				Math::DecomposeTransform( transform.WorldTransform( OwningScene().Registry(), entity ), position, rotation, scale );

				SpotLight spotLight;
				spotLight.Position = position;
				spotLight.Direction = rotation * Vector3::Forward(); 
				spotLight.Direction = spotLight.Direction.Normalized();
				spotLight.Intensity = spotLightComp.Intensity;
				spotLight.Color = spotLightComp.Color;
				spotLight.Range = spotLightComp.Range;
				spotLight.InnerConeCos = Math::Cos( Math::Radians( spotLightComp.InnerConeAngleDeg * 0.5f ) );
				spotLight.OuterConeCos = Math::Cos( Math::Radians( spotLightComp.OuterConeAngleDeg * 0.5f ) );
				spotLight.Falloff = spotLightComp.Falloff;
				spotLight.SourceSize = spotLightComp.SourceSize;
				spotLight.SpecularScale = spotLightComp.SpecularScale;
				lightEnv.SpotLights.PushBack( spotLight );
			} );

			RendererModule::GetPipelineManager()->SetLightEnvironment( std::move( lightEnv ) );
		}

		// Submit all static meshes to the render pipeline
		{
			OwningScene().Registry().Group<StaticMeshComponent>( EntityInclude<TransformComponent> ).Each(
			[&]( Entity entity, StaticMeshComponent& staticMesh, const TransformComponent& transform )
			{
				if ( staticMesh.Mesh.Valid() )
				{
					renderPipeline->SubmitStaticMesh( staticMesh.Mesh.GetOrLoad(), transform.WorldTransform( OwningScene().Registry(), entity ) );
				}
			} );
		}

	}

} // namespace Tridium