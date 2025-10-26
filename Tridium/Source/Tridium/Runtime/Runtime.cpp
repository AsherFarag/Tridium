#include "Runtime.h"

#if CONFIG_RUNTIME

#include <Tridium/Application/Application.h>
#include <Tridium/Scene/SceneManager.h>

// TEMP
#include <Tridium/Graphics/Renderer/RendererModule.h>
#include <Tridium/Graphics/RHI/RHI.h>

namespace Tridium {

	Runtime* Runtime::s_Instance = nullptr;

	void Runtime::OnAttach()
	{
		ENSURE( !s_Instance, "A Runtime instance already exists!" );

		s_Instance = this;

		Application::AddOnTick( TickGroups::BeginAppUpdate, []() { Get()->OnUpdate(); } );

		// TEMP!
		SceneManager::SetActiveScene( MakeShared<Scene>() );
	}

	void Runtime::OnDetach()
	{
		ENSURE( s_Instance == this, "Runtime instance mismatch on detach!" );

		s_Instance = nullptr;
	}

	void Runtime::OnEvent( Event& a_Event )
	{
	}

	void Runtime::OnUpdate()
	{
		RHITextureRef backBuffer = RHI::GetSwapChain()->GetBackBuffer();

		// TEMP!
		RenderView view{};
		view.Type = ERenderViewType::Camera;
		view.Constants.ProjectionMatrix = Math::Perspective( Math::Radians( 70.0f ), 16.0f / 9.0f, 0.1f, 1000.0f );
		view.Constants.ViewProjectionMatrix = view.Constants.ProjectionMatrix;
		view.Constants.ViewPosition = float4{ 0.0f, 0.0f, 0.0f, 1.0f };
		view.Constants.ViewportSize = { backBuffer->Desc().Width, backBuffer->Desc().Height };
		view.OutputTexture = backBuffer;
		RendererModule::GetPipelineManager()->AddView( view );

		SceneManager::ActiveScene()->OnTick( 0.016f ); // TODO: Delta Time
	}

} // namespace Tridium

#endif // CONFIG_RUNTIME