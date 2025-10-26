#include "EditorViewportPanel.h"

#if WITH_EDITOR

#include <Tridium/Editor/Editor.h>
#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Scene/Scene.h>
#include <ImGuizmo.h>

// TEMP
#include <Tridium/Math/Random.h>
#include <Tridium/Asset/AssetDatabase.h>
#include <Tridium/Asset/Importers/ModelImporter.h>
#include <Tridium/Graphics/Renderer/RendererComponents.h>
#include <Tridium/Asset/Importers/EnvironmentMapImporter.h>
#include <Tridium/Graphics/Renderer/RenderResourceManager.h>

namespace Tridium {

	// TEMP
	static Array<AssetRef<StaticMesh>> s_ImportedAssets;
	static uint32_t importedAssetIndex = 0;

	static void SetImGuizmoColors()
	{
		ImGuizmo::GetStyle() = ImGuizmo::Style();
		const UITheme& theme = UI::GetTheme();

		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_X] = theme.Red;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_Y] = theme.Green;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::DIRECTION_Z] = theme.Blue;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_X] = theme.Red;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_Y] = theme.Green;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::PLANE_Z] = theme.Blue;
		ImGuizmo::GetStyle().Colors[ImGuizmo::COLOR::SELECTION] = theme.Orange;
		ImGuizmo::GetStyle().RotationOuterLineThickness *= 2.0f;
		ImGuizmo::GetStyle().RotationLineThickness *= 2.0f;
		ImGuizmo::GetStyle().TranslationLineThickness *= 2.0f;
		//ImGuizmo::GetStyle().TranslationLineArrowSize *= 2.0f;
		//ImGuizmo::GetStyle().ScaleLineCircleSize *= 2.0f;
		ImGuizmo::GetStyle().ScaleLineThickness *= 2.0f;
	}

	EditorViewportPanel::EditorViewportPanel()
	{
				// TEMP

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
				importedEnvMapAsset = SharedPtrCast<EnvironmentMap>( context.m_CreatedAssets.Back() );

				lightEnv.Sky.EnvironmentMap = RenderResourceManager::GetOrCreateEnvironmentMap( importedEnvMapAsset );
			}

			RendererModule::GetPipelineManager()->SetLightEnvironment( std::move( lightEnv ) );
		}

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
				for ( const auto& asset : context.m_CreatedAssets )
				{
					LOG( LogCategory::Debug, Info, "Created Asset: {}", asset->Info()->Name );
					if ( asset->Type() == StaticMesh::StaticType() )
					{
						s_ImportedAssets.PushBack( SharedPtrCast<StaticMesh>( asset ) );
					}
				}
			};

			//ImportAsset( "TestProject/Content/damagedhelmet/DamagedHelmet.gltf" );
			//ImportAsset( "TestProject/Content/Sponza2/Sponza/glTF/Sponza.gltf" );
			//ImportAsset( "TestProject/Content/helljumper/scene.gltf" );
			//ImportAsset( "TestProject/Content/halo_5_recruit/scene.gltf" );
		}
	}

	void EditorViewportPanel::OnEvent( Event& a_Event )
	{
		EventDispatcher dispatcher( a_Event );
		dispatcher.Dispatch<KeyPressedEvent>( [this]( const KeyPressedEvent& a_Event ) -> bool { return Event_KeyPressed( a_Event ); } );
	}

	void EditorViewportPanel::OnUpdate( float a_DeltaTime )
	{
		if ( !m_ViewportTexture || 
			 m_ViewportTexture->Desc().Width != Cast<uint32_t>( m_ViewportSize.X ) || 
			 m_ViewportTexture->Desc().Height != Cast<uint32_t>( m_ViewportSize.Y ) )
		{
			RHITextureDesc textureDesc;
			textureDesc.Dimension = ERHITextureDimension::Texture2D;
			textureDesc.Width = Cast<uint32_t>( m_ViewportSize.X );
			textureDesc.Height = Cast<uint32_t>( m_ViewportSize.Y );
			textureDesc.Format = ERHIFormat::RGBA8_UNORM;
			textureDesc.BindFlags = ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource;
			textureDesc.UseClearValue = true;
			textureDesc.Name = "Editor Viewport Texture";
			m_ViewportTexture = RHI::CreateTexture( textureDesc );
		}

		RenderView view
		{
			.Constants{
				m_EditorCamera.GetViewMatrix(),
				m_EditorCamera.GetProjection(),
				Vector4{ m_EditorCamera.Position, 1.0f },
				Vector2{ m_ViewportSize.X, m_ViewportSize.Y },
				m_EditorCamera.GetPerspectiveFarClip(),
				m_EditorCamera.GetPerspectiveNearClip()
			},
			.Type = ERenderViewType::Camera,
			.Enabled = true,
			.OutputTexture = m_ViewportTexture,
		};

		RendererModule::GetPipelineManager()->AddView( view );
	}

	void EditorViewportPanel::OnDraw( StringView a_Name, bool& o_Open )
	{
		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2( 2.f, 2.f ) );

		if ( ImGui::Begin( a_Name.data() ) )
		{
			if ( ImGui::IsWindowHovered() && ImGui::IsMouseClicked( ImGuiMouseButton_Right ) )
				ImGui::SetWindowFocus();

			const Vector2 regionAvail = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
			const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			const auto viewportOffset = ImGui::GetWindowPos();
			const Vector2 viewportBoundsMin = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			const Vector2 viewportBoundsMax = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			// Update the viewport size
			m_ViewportSize = regionAvail;
			m_EditorCamera.SetViewportSize( m_ViewportSize.X, m_ViewportSize.Y );
			m_EditorCamera.OnUpdate();

			ImTextureID textureID = (ImTextureID)( m_ViewportTexture.get() );
			if ( textureID )
			{
				ImGui::Image( textureID, ImGui::GetContentRegionAvail() );
			}

			UI_DrawGizmo( viewportBoundsMin, viewportBoundsMax );
		}

		ImGui::End();
	}

	void EditorViewportPanel::UI_DrawGizmo( const Vector2& a_ViewportBoundsMin, const Vector2& a_ViewportBoundsMax )
	{
		const Matrix4& camProjection = m_EditorCamera.GetProjection();
		const Matrix4 oldCamView = m_EditorCamera.GetViewMatrix();
		Matrix4 camView = oldCamView;

		// Set up ImGuizmo
		SetImGuizmoColors();
		ImGuizmo::SetOrthographic( false );
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(
			a_ViewportBoundsMin.X,
			a_ViewportBoundsMin.Y,
			a_ViewportBoundsMax.X - a_ViewportBoundsMin.X,
			a_ViewportBoundsMax.Y - a_ViewportBoundsMin.Y
		);


		if ( TransformComponent* tc = Editor::GetSelectionContext().SelectedObject.TryGet<TransformComponent>() )
		{
			// Selected Game Object
			Matrix4 worldTransform = tc->LocalTransform();

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
				default: gizmoOperation = (ImGuizmo::OPERATION)0; break;
			}

			// Create a Manipulation Gizmo that allows the user to easily modify the Game-Object's transform
			ImGuizmo::Manipulate(
				&camView[0][0], &camProjection[0][0],
				gizmoOperation, ImGuizmo::LOCAL,
				&worldTransform[0][0], nullptr,
				shouldSnap ? &snapVals[0] : nullptr
			);

			if ( ImGuizmo::IsUsingAny() )
			{
				Quaternion rotation;
				Vector3 position;
				Vector3 scale;
				Math::DecomposeTransform( worldTransform, position, rotation, scale );
				tc->SetLocalPosition( position );
				tc->SetLocalRotation( rotation );
				tc->SetLocalScale( scale );
			}
		}

		ImGuizmo::ViewManipulate( &camView[0][0], 8.0f, { a_ViewportBoundsMax.X - 75, a_ViewportBoundsMin.Y }, { 75, 75 }, 0 );
	}

	bool EditorViewportPanel::Event_KeyPressed( const KeyPressedEvent& a_Event )
	{
		if ( a_Event.IsRepeat )
			return false;

		if ( !ImGuizmo::IsUsing() )
		{
			switch ( a_Event.KeyCode )
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
			}
		}

		return false;
	}


} // namespace Tridium

#endif // WITH_EDITOR