#include "RendererSceneSystem.h"
#include <Tridium/Graphics/Renderer/RendererComponents.h>
#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Asset/AssetDatabase.h>

namespace Tridium {

	void RendererSceneSystem::OnUpdate( float a_DeltaTime )
	{
		RenderPipelineManager* renderPipeline = RendererModule::GetPipelineManager();

		// Set up lighting environment
		{
			LightEnvironment lightEnv;

			// Set up skybox
			auto skyboxes = OwningScene().Registry().View<SkyboxComponent>();
			for ( EntityID entity : skyboxes )
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

			OwningScene().Registry().View<TransformComponent, PointLightComponent>().Each( [&]( EntityID entity, const TransformComponent& transform, const PointLightComponent& pointLightComp )
			{
				PointLight pointLight;
				pointLight.Position = transform.LocalPosition();
				pointLight.Color = pointLightComp.Color;
				pointLight.Intensity = pointLightComp.Intensity;
				pointLight.Radius = pointLightComp.Radius;
				lightEnv.PointLights.PushBack( pointLight );
			} );

			RendererModule::GetPipelineManager()->SetLightEnvironment( std::move( lightEnv ) );
		}

		// Submit all static meshes to the render pipeline
		{
			auto staticMeshes = OwningScene().Registry().View<TransformComponent, StaticMeshComponent>();
			staticMeshes.Each( [&]( EntityID entity, TransformComponent& transform, StaticMeshComponent& staticMesh )
			{
				if ( staticMesh.Mesh.Valid() )
				{
					renderPipeline->SubmitStaticMesh( staticMesh.Mesh.GetOrLoad(), transform.WorldTransform( OwningScene().Registry(), entity ) );
				}
			} );
		}

	}

} // namespace Tridium