#include "RendererSceneSystem.h"
#include <Tridium/Graphics/Renderer/RendererComponents.h>
#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/Asset/AssetDatabase.h>

namespace Tridium {

	void RendererSceneSystem::OnUpdate( float a_DeltaTime )
	{
		RenderPipelineManager* renderPipeline = RendererModule::GetPipelineManager();

		if ( false )
		{
			const auto AddCameraView = [renderPipeline]( StringView a_Name, const CameraComponent& a_Camera, const Matrix4 a_ViewMatrix )
			{
				RenderView view{};
				view.Type = ERenderViewType::Camera;
				view.Name = String( a_Name );
				TODO( "Im not sure if the renderviews should store the rhi format. And how should we get it?" );
				view.Camera.OutputFormat = ERHIFormat::RGBA16_UNORM;

				view.Constants.ViewMatrix = a_ViewMatrix;
				view.Constants.ProjectionMatrix = a_Camera.CalculateProjection();
				view.Constants.ViewProjectionMatrix = view.Constants.ProjectionMatrix * view.Constants.ViewMatrix;

				// Set view constants
				if ( a_Camera.ProjectionType == CameraComponent::EProjectionType::Perspective )
				{
					view.Constants.NearPlane = a_Camera.Perspective.NearPlane;
					view.Constants.FarPlane = a_Camera.Perspective.FarPlane;
				}
				else
				{
					view.Constants.NearPlane = a_Camera.Orthographic.NearPlane;
					view.Constants.FarPlane = a_Camera.Orthographic.FarPlane;
				}

				renderPipeline->AddView( std::move( view ) );
			};

			auto cameras = OwningScene().Registry().View<TransformComponent, CameraComponent>();
			cameras.Each( [&]( EntityID entity, TransformComponent& transform, CameraComponent& camera )
			{
				StringView name;

				if ( auto* tag = OwningScene().Registry().TryGet<TagComponent>( entity ) )
				{
					name = tag->Tag;
				}

				Matrix4 viewMatrix;

				TODO( "Handle world transforms" );
				viewMatrix = Math::Inverse( transform.LocalTransform() );

				AddCameraView( name, camera, viewMatrix );
			} );
		}

		// Submit all static meshes to the render pipeline
		{
			auto staticMeshes = OwningScene().Registry().View<TransformComponent, StaticMeshComponent>();
			staticMeshes.Each( [&]( EntityID entity, TransformComponent& transform, StaticMeshComponent& staticMesh )
			{
				if ( staticMesh.Mesh.Valid() )
				{
					renderPipeline->SubmitStaticMesh( staticMesh.Mesh.GetOrLoad(), transform.LocalTransform() );
				}
			} );
		}

	}

} // namespace Tridium