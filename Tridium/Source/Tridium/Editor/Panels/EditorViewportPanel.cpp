#include "tripch.h"

#if IS_EDITOR
#include "EditorViewportPanel.h"
#include <Tridium/Debug/DebugDrawer.h>
#include <Tridium/ECS/GameObject.h>
#include <Tridium/ECS/Components/Types.h>
#include <Tridium/Editor/Editor.h>
#include <Tridium/Editor/EditorUtil.h>
#include <Tridium/Editor/EditorCamera.h>
#include <Tridium/Editor/EditorStyle.h>
#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Scene/Scene.h>

#include <ImGuizmo.h>

// TEMP ?
#include <Tridium/Graphics/Renderer/RendererComponents.h>
#include <Tridium/Math/Random.h>
#include <Tridium/Graphics/oldRendering/SceneRenderer.h>
#include <Tridium/oldAsset/EditorAssetManager.h>
#include "Tridium/oldAsset/AssetManager.h"
#include <Tridium/Graphics/oldRendering/RenderCommand.h>
#include <Tridium/Graphics/oldRendering/VertexArray.h>
#include <Tridium/Graphics/oldRendering/Shader.h>
#include <Tridium/Graphics/Renderer/SceneRenderer.h>
#include <Tridium/ImGui/ImGuiModule.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Reflection/FieldReflection.h>
#include <Tridium/Asset/AssetDatabase.h>
#include <Tridium/Asset/Importers/ModelImporter.h>
#include <Tridium/Asset/Importers/EnvironmentMapImporter.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>
#include <Tridium/Graphics/Renderer/HighDefinitionRenderPipeline.h>

namespace Tridium {

	void SetImGuizmoColors()
	{
		ImGuizmo::GetStyle() = ImGuizmo::Style();
		EditorStyle::Pallete& pal = Editor::GetStyle().Colors;

		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_X] = pal.Red;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_Y] = pal.Green;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_Z] = pal.Blue;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_X] = pal.Red;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_Y] = pal.Green;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_Z] = pal.Blue;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::SELECTION] = pal.Orange;
		ImGuizmo::GetStyle().RotationOuterLineThickness *= 2.0f;
		ImGuizmo::GetStyle().RotationLineThickness *= 2.0f;
		ImGuizmo::GetStyle().TranslationLineThickness *= 2.0f;
		//ImGuizmo::GetStyle().TranslationLineArrowSize *= 2.0f;
		//ImGuizmo::GetStyle().ScaleLineCircleSize *= 2.0f;
		ImGuizmo::GetStyle().ScaleLineThickness *= 2.0f;
	}

	OldEditorViewportPanel::OldEditorViewportPanel( const SharedPtr<EditorCamera>& editorCamera )
		: ViewportPanel( "Scene##EditorViewportPanel" ), m_EditorCamera( editorCamera )
	{
		m_OnGameObjectSelectedHandle = Editor::Events::OnGameObjectSelected.Add<&OldEditorViewportPanel::SetSelectedGameObject>( this );
	}

	OldEditorViewportPanel::~OldEditorViewportPanel()
	{
	}

	bool OldEditorViewportPanel::OnKeyPressed( KeyPressedEvent& e )
	{
		if ( e.IsRepeat )
			return false;

		if ( !ImGuizmo::IsUsing() )
		{
			switch ( e.KeyCode )
			{
			case EInputKey::E:
			{
				m_GizmoState = EGizmoState::Translate;
				return true;
			}
			case EInputKey::R:
			{
				m_GizmoState = EGizmoState::Rotate;
				return true;
			}
			case EInputKey::T:
			{
				if ( Input::IsKeyPressed( EInputKey::LeftControl ) )
					m_GizmoState = EGizmoState::UniversalScale;
				else
					m_GizmoState = EGizmoState::Scale;

				return true;
			}
			case EInputKey::C:
			{
				if ( Input::IsKeyPressed( EInputKey::LeftControl ) )
				{
					if ( m_SelectedGameObject )
					{
						Editor::GetPayloadManager().SetPayload( "GameObject", m_SelectedGameObject );
					}
				}
				return true;
			}
			case EInputKey::V:
			{
				if ( Input::IsKeyPressed( EInputKey::LeftControl ) )
				{
					EditorPayload* payload = Editor::GetPayloadManager().GetPayload( "GameObject" );
					if ( payload && !payload->IsEmpty() )
					{
						OldGameObject go = payload->As<OldGameObject>();
						OldGameObject newGO = OldSceneManager::GetActiveScene()->InstantiateGameObjectFrom(go);
						Editor::Events::OnGameObjectSelected.Broadcast( newGO );
					}
				}
				return true;
			}
			case EInputKey::Delete:
			{
				if ( m_SelectedGameObject )
				{
					m_SelectedGameObject.Destroy();
					Editor::Events::OnGameObjectSelected.Broadcast( OldGameObject() );
				}
				return true;
			}
			}
		}

		return false;
	}

	static RenderViewID s_ViewID = 0;
	static Array<AssetRef<StaticMesh>> s_ImportedAssets;
	static uint32_t importedAssetIndex = 0;
	static Scene s_TestScene;
	static GameObject s_TestGO;

	void OldEditorViewportPanel::OnUpdate( float a_DeltaTime )
	{
		if ( !m_EditorCamera || m_ViewportSize.X <= 0 || m_ViewportSize.Y <= 0 )
			return;

		static GameObject TestGO = []()
		{
			s_TestScene.Init();
			GameObject test = s_TestScene.CreateEmptyGameObject();
			test.Add<TransformComponent>();
			test.Add<StaticMeshComponent>();

			s_TestScene.OnBeginPlay( EScenePlayMode::Play );
			s_TestGO = test;
			return test;
		}( );

		RenderView view
		{
			.Constants{
				m_EditorCamera->GetViewMatrix(),
				m_EditorCamera->GetProjection(),
				Vector4{ m_EditorCamera->Position, 1.0f },
				Vector2{ m_ViewportSize.X, m_ViewportSize.Y },
				m_EditorCamera->GetPerspectiveFarClip(),
				m_EditorCamera->GetPerspectiveNearClip()
			},
			.Type = ERenderViewType::Camera,
			.Enabled = true,
			.Camera = {.OutputFormat = ERHIFormat::RGBA16_UNORM }
		};

		{
			//TEMP
			//const FilePath assetFilePath = "TestProject/Content/troll/troll/TrollApose_low.fbx";
			auto modelImporter = AssetFactory::GetImporter( ".fbx" );
			static bool imported = false;

			if ( modelImporter && !imported )
			{
				imported = true;

				const auto ImportAsset = [&]( const FilePath& path )
				{
					AssetImportContext context;
					context.m_AssetPath = path;
					modelImporter->OnImport( context );
					for ( const auto& [metaData, asset] : context.m_CreatedAssets )
					{
						LOG( LogCategory::Debug, Info, "Created Asset: {}", metaData.Name );
						if ( asset->Type() == StaticMesh::StaticType() )
						{
							s_ImportedAssets.PushBack( SharedPtrCast<StaticMesh>( asset ) );
						}
					}
				};

				ImportAsset( "TestProject/Content/damagedhelmet/DamagedHelmet.gltf" );
				ImportAsset( "TestProject/Content/Sponza2/Sponza/glTF/Sponza.gltf" );
				//ImportAsset( "TestProject/Content/helljumper/scene.gltf" );
				//ImportAsset( "TestProject/Content/halo_5_recruit/scene.gltf" );
			}

			if ( s_ImportedAssets.IsValidIndex( importedAssetIndex ) )
			{
				TestGO.Get<StaticMeshComponent>().Mesh = AssetHandle<StaticMesh>( s_ImportedAssets[importedAssetIndex] );
				//RendererModule::GetPipelineManager()->SubmitStaticMesh( s_ImportedAssets[importedAssetIndex], Matrix4{ 1.0f } );
			}

			s_TestScene.OnTick( a_DeltaTime );
		}

		// Temp
		static bool SetLightEnv = []() -> bool
		{
			static Array<PointLight> s_PointLights = []() -> Array<PointLight>
			{
				Array<PointLight> lights;
				lights.Reserve( 32 );

				for ( int i = 0; i < 32; i++ )
				{
					PointLight light;
					light.Position = Vector3{
						Math::Random::Range( -10.0f, 10.0f ),
						Math::Random::Range( 0.0f, 5.0f ),
						Math::Random::Range( -10.0f, 10.0f )
					};
					light.Color = Vector3{
						Math::Random::Range( 0.0f, 1.0f ),
						Math::Random::Range( 0.0f, 1.0f ),
						Math::Random::Range( 0.0f, 1.0f )
					};
					light.Intensity = Math::Random::Range( 2.0f, 5.0f );
					light.Radius = Math::Random::Range( 1.0f, 5.0f ) + light.Intensity * 0.5f;
					lights.PushBack( light );
				}

				return lights;
			}( );

			LightEnvironment lightEnv;
			lightEnv.PointLights = std::move( s_PointLights );

			{
				//const FilePath assetFilePath = "TestProject/Content/resting_place_2_4k.hdr";
				//const FilePath assetFilePath = "TestProject/Content/studio_small.hdr";
				const FilePath assetFilePath = "TestProject/Content/park_music_stage_4k.hdr";
				auto envMapImporter = AssetFactory::GetImporter( assetFilePath.GetExtension().ToString() );
				static AssetRef<EnvironmentMap> importedEnvMapAsset;
				AssetImportContext context;
				context.m_AssetPath = assetFilePath;
				envMapImporter->OnImport( context );
				importedEnvMapAsset = SharedPtrCast<EnvironmentMap>( context.m_CreatedAssets.Back().second );

				lightEnv.Sky.EnvironmentMap = RenderResourceManager::GetOrCreateEnvironmentMap( importedEnvMapAsset );
			}

			RendererModule::GetPipelineManager()->SetLightEnvironment( std::move( lightEnv ) );

			return true;
		}( );

		s_ViewID = RendererModule::GetPipelineManager()->AddView( view );
	}

	void OldEditorViewportPanel::OnImGuiDraw()
	{
		if ( !m_EditorCamera )
			return;

		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 2.f, 2.f ) );

		if ( ImGui::Begin( m_Name.c_str() ) )
		{
			if ( ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) )
				ImGui::SetWindowFocus();

			const Vector2 regionAvail = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
			const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			const auto viewportOffset = ImGui::GetWindowPos();
			const Vector2 viewportBoundsMin = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			const Vector2 viewportBoundsMax = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			// Update the viewport size
			m_ViewportSize = regionAvail;
			m_EditorCamera->SetViewportSize( m_ViewportSize.X, m_ViewportSize.Y );
			m_EditorCamera->OnUpdate();
			ImTextureID textureID = (ImTextureID)( RendererModule::GetPipelineManager()->GetViewOutput( s_ViewID ).get() );
			if ( textureID )
			{
				ImGui::Image( textureID, ImGui::GetContentRegionAvail() );
			}

			DrawManipulationGizmos( viewportBoundsMin, viewportBoundsMax );
		}

		m_IsHovered = ImGui::IsWindowHovered();
		m_IsFocused = ImGui::IsWindowFocused();
		m_EditorCamera->Focused = m_IsFocused && !ImGuizmo::IsUsingAny();
		ImGui::End();

		ImGui::Begin( "Render Pipeline Debug" );
		{
			using Passes = HighDefinitionRenderPipeline::Passes;
			auto& renderer = *RendererModule::GetPipelineManager()->GetRenderPipeline();
			auto* pass = renderer.GetRenderPass<GBufferPipelinePass>( Passes::GBuffer );
			if ( pass && pass->GetAlbedoTexture() )
			{
				ImVec2 size = { (float)pass->GetAlbedoTexture()->Desc().Width, (float)pass->GetAlbedoTexture()->Desc().Height };
				const ImVec2 maxSize = ImGui::GetContentRegionAvail();
				if ( size.x > maxSize.x )
				{
					float aspect = size.y / size.x;
					size.x = maxSize.x;
					size.y = size.x * aspect;
				}
			
				if ( size.y > maxSize.y )
				{
					float aspect = size.x / size.y;
					size.y = maxSize.y;
					size.x = size.y * aspect;
				}
			
				ImGui::Text( "GBuffer Pass Textures:" );
				ImGui::Separator();
				ImGui::Text( "Position:" );
				ImGui::Image( (ImTextureID)pass->GetPositionTexture().get(), size );
				ImGui::Text( "Albedo:" );
				ImGui::Image( (ImTextureID)pass->GetAlbedoTexture().get(), size );
				ImGui::Text( "Normal:" );
				ImGui::Image( (ImTextureID)pass->GetNormalTexture().get(), size );
				ImGui::Text( "MetallicRoughnessAO:" );
				ImGui::Image( (ImTextureID)pass->GetMRAOTexture().get(), size );
				ImGui::Text( "Emission:" );
				ImGui::Image( (ImTextureID)pass->GetEmissionTexture().get(), size );
			}
		}
		ImGui::End();

		if ( ImGui::Begin( "Render Debug" ) )
		{
			ImGui::Checkbox( "Enable Lights", &g_TestLightEnable );
			ImGui::Checkbox( "Draw Debug Lights", &g_TestDrawLights );

			int assetindex = (int)importedAssetIndex;
			ImGui::InputInt( "Imported Asset Index", &assetindex );
			importedAssetIndex = (uint32_t)assetindex;
			importedAssetIndex = s_ImportedAssets.Size() > 0 ? importedAssetIndex % s_ImportedAssets.Size() : 0;
		}
		ImGui::End();
	}

	void OldEditorViewportPanel::DragDropTarget()
	{
		ImGui::ScopedDragDropTarget scopedDragDropTarget;
		if ( !( scopedDragDropTarget ) )
			return;

		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( TE_PAYLOAD_ASSET_HANDLE, ImGuiDragDropFlags_::ImGuiDragDropFlags_SourceAllowNullID );
		if ( !payload )
			return;

		OldAssetHandle assetHandle( *(OldAssetHandle*)payload->Data );

		const OldAssetMetaData& assetMetaData = EditorAssetManager::Get()->GetAssetMetaData( assetHandle );

		switch ( assetMetaData.AssetType )
		{
		case EAssetTypeOld::Scene:
		{
			if ( SharedPtr<OldScene> scene = AssetManager::GetAsset<OldScene>( assetHandle ) )
			{
				OldSceneManager::GetActiveScene()->Clear();
				// Load the scene
				OldSceneManager::SetActiveScene( scene.get() );
			}
			break;
		}
		case EAssetTypeOld::StaticMesh:
		{
			if ( SharedPtr<OldStaticMesh> mesh = AssetManager::GetAsset<OldStaticMesh>( assetHandle ) )
			{
				OldGameObject go = OldSceneManager::GetActiveScene()->InstantiateGameObject();
				go.AddComponent<OldStaticMeshComponent>().Mesh = mesh->GetHandle();

				Vector3 position = m_EditorCamera->Position + m_EditorCamera->GetForwardDirection() * 5.0f;

				go.GetTransform().SetWorldPosition( position );

				Editor::Events::OnGameObjectSelected.Broadcast( go );
			}
			break;
		}
		default:
			break;
		}
	}

	void OldEditorViewportPanel::DrawManipulationGizmos( const Vector2& viewportBoundsMin, const Vector2& viewportBoundsMax )
	{
		const Matrix4& camProjection = m_EditorCamera->GetProjection();
		const Matrix4 oldCamView = m_EditorCamera->GetViewMatrix();
		Matrix4 camView = oldCamView;

		// Set up ImGuizmo
		SetImGuizmoColors();
		ImGuizmo::SetOrthographic( false );
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(
			viewportBoundsMin.X,
			viewportBoundsMin.Y,
			viewportBoundsMax.X - viewportBoundsMin.X,
			viewportBoundsMax.Y - viewportBoundsMin.Y
		);


		if ( s_TestGO )
		{
			// Selected Game Object
			TransformComponent& goTransform = s_TestGO.Get<TransformComponent>();
			Matrix4 goWorldTransform = goTransform.LocalTransform();

			bool shouldSnap = Input::IsKeyPressed( EInputKey::LeftControl );

			Vector3 snapVals( 0.25f );
			if ( m_GizmoState == EGizmoState::Rotate )
				snapVals = Vector3( 45.0f );

			// Convert the Gizmo operation to ImGuizmo operation
			ImGuizmo::OPERATION gizmoOperation;
			switch ( m_GizmoState )
			{
				case EGizmoState::Translate: gizmoOperation = ImGuizmo::TRANSLATE; break;
				case EGizmoState::Rotate: gizmoOperation = ImGuizmo::ROTATE; break;
				case EGizmoState::Scale: gizmoOperation = ImGuizmo::SCALE; break;
				case EGizmoState::UniversalScale: gizmoOperation = ImGuizmo::SCALEU; break;
				default: gizmoOperation = ( ImGuizmo::OPERATION )0; break;
			}

			// Create a Manipulation Gizmo that allows the user to easily modify the Game-Object's transform
			ImGuizmo::Manipulate(
				&camView[0][0], &camProjection[0][0],
				gizmoOperation, ImGuizmo::LOCAL,
				&goWorldTransform[0][0], nullptr,
				shouldSnap ? &snapVals[0] : nullptr 
			);

			ImGuizmo::ViewManipulate( &camView[0][0], 8.0f, { viewportBoundsMax.X - 75, viewportBoundsMin.X }, { 75, 75 }, 0x10101010 );

			if ( ImGuizmo::IsUsingAny() )
			{
				Quaternion rotation;
				Vector3 position;
				Vector3 scale;
				Math::DecomposeTransform( goWorldTransform, position, rotation, scale );
				goTransform.SetLocalPosition( position );
				goTransform.SetLocalRotation( rotation );
				goTransform.SetLocalScale( scale );
			}
		}
		else
		{
			Matrix4 identity( 1.0f );
			ImGuizmo::ViewManipulate( &camView[0][0], &camProjection[0][0],
				(ImGuizmo::OPERATION)m_GizmoState, ImGuizmo::LOCAL,
				&identity[0][0], 8.0f, ImVec2( viewportBoundsMax.X - 75, viewportBoundsMin.X ), ImVec2( 75, 75 ), 0x10101010 );
		}

		// Update the Editor Camera's View Matrix
		if ( oldCamView != camView )
			m_EditorCamera->SetViewMatrix( camView );
	}

	void OldEditorViewportPanel::RenderGameObjectIDs()
	{
		RenderCommand::SetDepthTest( true );
		RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );
		RenderCommand::SetCullMode( ECullMode::Back );
		RenderCommand::SetClearColor( { 0.1, 0.1, 0.12, 1.0 } );
		RenderCommand::Clear();

		m_GameObjectIDShader->Bind();

		Matrix4 pvm = m_EditorCamera->GetProjection() * m_EditorCamera->GetViewMatrix();

		auto meshComponents = OldSceneManager::GetActiveScene()->GetECS().View<OldStaticMeshComponent, OldTransformComponent>();
		meshComponents.each( 
			[&]( auto go, OldStaticMeshComponent& meshComponent, OldTransformComponent& transform )
			{
				if ( !meshComponent.Mesh.IsValid() )
					return;

				SharedPtr<OldStaticMesh> mesh = AssetManager::GetAsset<OldStaticMesh>( meshComponent.Mesh );
				if ( !mesh )
					return;

				SharedPtr<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>( mesh->GetMeshSource() );
				if ( !meshSource )
					return;

				m_GameObjectIDShader->SetInt( "uID", Cast<uint32_t>( go ) );
				for ( uint32_t subMeshIndex : mesh->GetSubMeshes() )
				{
					const OldSubMesh& subMesh = meshSource->GetSubMeshes()[subMeshIndex];
					if ( const SharedPtr<VertexArray>& vao = subMesh.VAO )
					{
						m_GameObjectIDShader->SetMatrix4( "uPVM", pvm * transform.GetWorldTransform() * subMesh.Transform );

						vao->Bind();
						RenderCommand::DrawIndexed( vao );
						vao->Unbind();
					}
				}
			} );

		m_GameObjectIDShader->Unbind();
	}

	void OldEditorViewportPanel::RenderSelectionOutline()
	{
	#if 0
		if ( !m_FBO || !m_SelectedGameObject.IsValid() )
			return;

		OldStaticMeshComponent* meshComponent = m_SelectedGameObject.TryGetComponent<OldStaticMeshComponent>();
		if ( !meshComponent )
			return;

		if ( !meshComponent->Mesh.IsValid() )
			return;

		SharedPtr<StaticMesh> mesh = AssetManager::GetAsset<StaticMesh>( meshComponent->Mesh );
		if ( !mesh )
			return;

		SharedPtr<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>( mesh->GetMeshSource() );
		if ( !meshSource )
			return;

		RenderCommand::SetDepthCompare( EDepthCompareOperator::Less );
		RenderCommand::SetCullMode( ECullMode::Front );

		m_FBO->Bind();
		m_OutlineShader->Bind();
		m_OutlineShader->SetFloat4( "u_Color", { 1.0f, 0.85f, 0.0f, 1.0f } );

		RenderCommand::SetLineWidth( 5 );
		RenderCommand::SetPolygonMode( EFaces::FrontAndBack, EPolygonMode::Line );

		Matrix4 pvm = m_EditorCamera->GetProjection() * m_EditorCamera->GetViewMatrix();
		for ( uint32_t subMeshIndex : mesh->GetSubMeshes() )
		{
			const SubMesh& subMesh = meshSource->GetSubMeshes()[subMeshIndex];
			if ( const SharedPtr<VertexArray>& vao = subMesh.VAO )
			{
				m_OutlineShader->SetMatrix4( "u_PVM", pvm * m_SelectedGameObject.GetWorldTransform() * subMesh.Transform );

				vao->Bind();
				RenderCommand::DrawIndexed( vao );
				vao->Unbind();
			}
		}

		m_OutlineShader->Unbind();
		m_FBO->Unbind();

		RenderCommand::SetPolygonMode( EFaces::FrontAndBack, EPolygonMode::Fill );

	#endif
	}
}

#endif // IS_EDITOR
