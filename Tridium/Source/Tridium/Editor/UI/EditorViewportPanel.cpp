#include "EditorViewportPanel.h"

#if WITH_EDITOR

#include <Tridium/Editor/Editor.h>
#include <Tridium/Editor/UserActions/SceneActions.h>
#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Scene/Scene.h>
#include <ImGuizmo.h>

// TEMP
#include <Tridium/Asset/AssetDatabase.h>
#include <Tridium/Graphics/Renderer/RendererComponents.h>

namespace Tridium {

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
		AssetDatabase::ImportAsset( "TestProject/Content/park_music_stage_4k.hdr" );
		AssetDatabase::ImportAsset( "TestProject/Content/damagedhelmet/DamagedHelmet.gltf" );
		//AssetDatabase::ImportAsset( "TestProject/Content/Sponza2/Sponza/glTF/Sponza.gltf" );
		//AssetDatabase::ImportAsset( "TestProject/Content/helljumper/scene.gltf" );
		AssetDatabase::ImportAsset( "TestProject/Content/halo_5_recruit/scene.gltf" );
	}

	void EditorViewportPanel::OnEvent( Event& a_Event )
	{
		EventDispatcher dispatcher( a_Event );
		dispatcher.Dispatch<KeyPressedEvent>( [this]( const KeyPressedEvent& a_Event ) -> bool { return Event_KeyPressed( a_Event ); } );
	}

	void EditorViewportPanel::OnUpdate( float a_DeltaTime )
	{
		m_PreviewObject = {};
		if ( const auto& selections = SelectionManager::Get().Selections( ESelectionContext::Scene ); selections.Size() > 0 )
		{
			// Find the first selected GameObject with a Camera component
			for ( const Selectable& selected : selections )
			{
				if ( !std::holds_alternative<GameObject>( selected ) )
					continue;

				GameObject gameObject = std::get<GameObject>( selected );
				if ( !gameObject.Valid() )
					continue;

				if ( gameObject.Has<CameraComponent>() )
				{
					m_PreviewObject = gameObject;
					break;
				}
			}
		}

		m_EditorCamera.Focused = m_ViewportFocused;
		m_EditorCamera.SetViewportSize( m_ViewportSize.X, m_ViewportSize.Y );
		m_EditorCamera.OnUpdate();

		// Create or resize the viewport texture if needed
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

		// Submit the render view for the editor camera
		{
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

		m_PreviewTexture = nullptr;
		if ( m_PreviewObject.Valid() )
		{
			if ( CameraComponent* cameraComp = m_PreviewObject.TryGet<CameraComponent>() )
			{
				Vector2 viewportSize = cameraComp->ViewportSize * 0.2f;
				viewportSize = Math::Max( Vector2::One(), viewportSize );

				RHITextureDesc previewTextureDesc;
				previewTextureDesc.Dimension = ERHITextureDimension::Texture2D;
				previewTextureDesc.Width = Cast<uint32_t>( viewportSize.X );
				previewTextureDesc.Height = Cast<uint32_t>( viewportSize.Y );
				previewTextureDesc.Format = ERHIFormat::RGBA8_UNORM;
				previewTextureDesc.BindFlags = ERHIBindFlags::RenderTarget | ERHIBindFlags::ShaderResource;
				previewTextureDesc.UseClearValue = true;
				previewTextureDesc.Name = "Editor Preview Viewport Texture";
				m_PreviewTexture = RHI::CreateTexture( previewTextureDesc );

				Matrix4 view = Math::Inverse(
					Math::Translate( m_PreviewObject.GetWorldPosition() )
					* Math::ToMat4( m_PreviewObject.GetWorldRotation() ) 
				);


				RendererModule::GetPipelineManager()->AddCameraView(
					m_PreviewTexture,
					viewportSize,
					m_PreviewObject.GetWorldPosition(),
					view,
					cameraComp->CalculateProjection(),
					cameraComp->NearPlane(),
					cameraComp->FarPlane(),
					"Editor Preview Camera View"
				);
			}
		}
	}

	void EditorViewportPanel::OnDraw( StringView a_Name, bool& o_Open )
	{
		ImGui::ScopedStyleVar winPadding( ImGuiStyleVar_WindowPadding, ImVec2( 2.f, 2.f ) );

		if ( ImGui::Begin( a_Name.data() ) )
		{
			if ( ImGui::IsWindowHovered() && ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseClicked( ImGuiMouseButton_Right ) ) )
			{
				ImGui::SetWindowFocus();
			}

			m_ViewportFocused = ImGui::IsWindowFocused();

			const Vector2 regionAvail = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
			const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			const auto viewportOffset = ImGui::GetWindowPos();
			const Vector2 viewportBoundsMin = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			const Vector2 viewportBoundsMax = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			// Update the viewport size
			m_ViewportSize = regionAvail;

			// Draw the viewport texture
			if ( m_ViewportTexture )
			{
				ImGui::Image( (ImTextureID)( m_ViewportTexture.get() ), ImGui::GetContentRegionAvail() );
			}

			// Draw camera preview 
			if ( m_PreviewObject.Valid() && m_PreviewTexture )
			{
				Vector2 previewViewportSize = m_ViewportSize;
				previewViewportSize *= 0.2f; // 20% of the main viewport size

				const Vector2 previewViewportMin = viewportBoundsMax - previewViewportSize - Vector2( 10.0f );
				const Vector2 previewViewportMax = viewportBoundsMax - Vector2( 10.0f );

				ImGui::SetCursorScreenPos( ImGui::Convert( previewViewportMin ) );

				ImGui::Image( m_PreviewTexture.get(), ImGui::Convert( previewViewportSize ) );

				// Add a border to the preview viewport
				const UITheme& theme = UI::GetTheme();
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				drawList->AddRect( ImGui::Convert( previewViewportMin ) - ImVec2( theme.SelectedBorderSize / 2, theme.SelectedBorderSize / 2 ),
								   ImGui::Convert( previewViewportMax ) + ImVec2( theme.SelectedBorderSize / 2, theme.SelectedBorderSize / 2 ),
								   theme.BrightText, theme.SelectedRounding, 0, theme.SelectedBorderSize );
			}

			// Draw transformation gizmos
			UI_DrawGizmo( viewportBoundsMin, viewportBoundsMax );

			// Draw a border around the viewport if it's focused
			if ( m_ViewportFocused )
			{
				const UITheme& theme = UI::GetTheme();

				ImDrawList* drawList = ImGui::GetForegroundDrawList( ImGui::GetCurrentWindow() );
				ImVec2 borderOffset = ImVec2( theme.SelectedBorderSize / 2, theme.SelectedBorderSize / 2 );
				drawList->AddRect( ImGui::Convert( viewportBoundsMin ) - borderOffset,
								   ImGui::Convert( viewportBoundsMax ) + borderOffset,
								   theme.Selected, theme.SelectedRounding, 0, theme.SelectedBorderSize );
			}
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

		for ( const Selectable& selectedObject : SelectionManager::Get().Selections( ESelectionContext::Scene ) )
		{
			if ( !std::holds_alternative<GameObject>( selectedObject ) )
				continue;

			GameObject gameObject = std::get<GameObject>( selectedObject );

			if ( TransformComponent* tc = gameObject.TryGet<TransformComponent>() )
			{
				// Selected Game Object
				Matrix4 worldTransform = tc->WorldTransform( gameObject.Scene()->Registry(), gameObject );

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
					if ( !m_WasUsingGizmoLastFrame )
					{
						Editor::GetUserActionManager().Push( 
							MakeUnique<ComponentUserAction<TransformComponent>>( 
								gameObject, 
								EComponentUserActionType::Modify 
							)
						);

						m_WasUsingGizmoLastFrame = true;
					}

					// Convert the modified world transform back to local transform
					Matrix4 parentWorldTransform{ 1.0f };
					if ( const auto* hierarchy = gameObject.TryGet<HierarchyComponent>() )
					{
						GameObject parentGameObject{ gameObject.Scene(), hierarchy->Parent };
						if ( parentGameObject.Valid() )
						{
							if ( TransformComponent* parentTc = parentGameObject.TryGet<TransformComponent>() )
							{
								parentWorldTransform = parentTc->WorldTransform( parentGameObject.Scene()->Registry(), parentGameObject );
							}
						}
					}

					Matrix4 localTransform = Math::Inverse( parentWorldTransform ) * worldTransform;

					Quaternion rotation;
					Vector3 position;
					Vector3 scale;
					Math::DecomposeTransform( localTransform, position, rotation, scale );
					tc->SetLocalPosition( position );
					tc->SetLocalRotation( rotation );
					tc->SetLocalScale( scale );
				}
				else
				{
					m_WasUsingGizmoLastFrame = false;
				}
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