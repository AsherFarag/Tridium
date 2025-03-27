#include "tripch.h"
#include "Application.h"
#include <Tridium/Engine/Engine.h>
#include <Tridium/IO/FileManager.h>
#include <Tridium/Debug/Profiler/ProfileSessionSerializer.h>

// TEMP!
#if IS_EDITOR
	#include <Tridium/Editor/Editor.h>
#endif // IS_EDITOR

#include <Tridium/ImGui/ImGuiLayer.h>

// TEMP ?
#include <Tridium/Graphics/Rendering/GameViewport.h>
#include <Tridium/Graphics/Rendering/RenderCommand.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Graphics/RHI/RHIShaderCompiler.h>
#include <Tridium/Math/MathConstants.h>
#include <Tridium/Graphics/RHI/Backend/DirectX12/D3D12DynamicRHI.h>
#include <GLFW/glfw3.h>
#include <Tridium/Reflection/FieldReflection.h>

import Tridium.ECS;

namespace Tridium {

	Application* Application::s_Instance = nullptr;

	///////////////////////////////////////////////////////////////////////////////////////////

	Application::Application( CmdLineArgs a_ProjectPath )
	{
		s_Instance = this;
		m_CommandLineArgs = std::move( a_ProjectPath );
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
		s_Instance = nullptr;
	}
	
	void Application::Run()
	{
		Init();

		m_Running = true;

		m_GameViewport.Init( m_Window->GetWidth(), m_Window->GetHeight() );

		uint32_t frameCounter = 0;
		double fpsInterval = 0.0;
		uint32_t minFPS = 0xFFFFFFFF;
		uint32_t maxFPS = 0;

	#if !IS_EDITOR
		if ( m_ActiveScene )
		{
			m_ActiveScene->OnBeginPlay();
		}
	#endif // IS_EDITOR

		while ( m_Running )
		{
			PROFILE_FRAME();
			// Update Time
			const double lastFrameTime = Time::Now();
			Time::Update(); // 	s_Time = glfwGetTime();
			Time::s_DeltaTime = Time::Now() - lastFrameTime;

			// Update FPS
			++frameCounter;
			fpsInterval += Time::DeltaTime();
			uint32_t curFrameRate = 1.0 / Time::DeltaTime();
			minFPS = Math::Min( minFPS, curFrameRate );
			maxFPS = Math::Max( maxFPS, curFrameRate );
			if ( fpsInterval >= 1.0 )
			{
				m_PrevFrameInfo.FPS = frameCounter;
				m_PrevFrameInfo.MinFPS = minFPS;
				m_PrevFrameInfo.MaxFPS = maxFPS;
				frameCounter = 0;
				fpsInterval = 0.0;
				minFPS = 0xFFFFFFFF;
				maxFPS = 0;
			}

			// Update Loop
			if ( !m_Window->IsMinimized() )
			{
				Update();
			}

			m_Window->OnUpdate();
		}

	#if !IS_EDITOR
		if ( m_ActiveScene )
		{
			m_ActiveScene->OnEndPlay();
		}
	#endif // IS_EDITOR

		Shutdown();
	}

	bool Application::Init()
	{
		// Initialise the Window
		WindowProps props;
		props.Width = 1280;
		props.Height = 720;
		m_Window = Window::Create( props );
		m_Window->SetEventCallback( [this]( Event& a_Event ) { this->OnEvent( a_Event ); } );

		RHIConfig config;
		config.RHIType = ERHInterfaceType::DirectX12;
		config.UseDebug = true;
		bool initSuccess = RHI::Initialise( config );
		LOG( LogCategory::RHI, Info, "'{0}' - RHI: Initialised = {1}", RHI::GetRHIName( config.RHIType ), initSuccess );

		// TEMP!
#if 1
		{
			uint8_t testImgData[64 * 64 * 4];
			//for ( size_t y = 0; y < 64; y++ )
			//{
			//	for ( size_t x = 0; x < 64; x++ )
			//	{
			//		testImgData[( y * 64 + x ) * 4 + 0] = x * 4;
			//		testImgData[( y * 64 + x ) * 4 + 1] = y * 4;
			//		testImgData[( y * 64 + x ) * 4 + 2] = 255 - ( x * 2 + y * 2 );
			//		testImgData[( y * 64 + x ) * 4 + 3] = 255;
			//	}
			//}

			for ( int y = 0; y < 64; y++ )
			{
				for ( int x = 0; x < 64; x++ )
				{
					if ( y < 64 / 3 )
					{
						testImgData[( y * 64 + x ) * 4 + 0] = 255;
						testImgData[( y * 64 + x ) * 4 + 1] = 0;
						testImgData[( y * 64 + x ) * 4 + 2] = 0;
						testImgData[( y * 64 + x ) * 4 + 3] = 255;
					}
					else if ( y < 64 / 3 * 2 )
					{
						testImgData[( y * 64 + x ) * 4 + 0] = 0;
						testImgData[( y * 64 + x ) * 4 + 1] = 255;
						testImgData[( y * 64 + x ) * 4 + 2] = 0;
						testImgData[( y * 64 + x ) * 4 + 3] = 255;
					}
					else
					{
						testImgData[( y * 64 + x ) * 4 + 0] = 0;
						testImgData[( y * 64 + x ) * 4 + 1] = 0;
						testImgData[( y * 64 + x ) * 4 + 2] = 255;
						testImgData[( y * 64 + x ) * 4 + 3] = 255;
					}
				}
			}

			// - Create a texture -
			RHITextureDescriptor desc;
			desc.InitialData = testImgData; // std::span<const uint8_t> 
			desc.Width = 64;
			desc.Height = 64;
			desc.Format = ERHITextureFormat::RGBA8;
			desc.Name = "My texture";
			RHITextureRef tex = RHI::CreateTexture( desc );

			// - Create a sampler -
			RHISamplerDescriptor samplerDesc;
			samplerDesc.Filter = ERHISamplerFilter::Bilinear;
			samplerDesc.AddressU = ERHISamplerAddressMode::Clamp;
			samplerDesc.AddressV = ERHISamplerAddressMode::Clamp;
			samplerDesc.AddressW = ERHISamplerAddressMode::Clamp;
			samplerDesc.Name = "My sampler";
			RHISamplerRef sampler = RHI::CreateSampler( samplerDesc );

			// Set the sampler of the texture ( Optional if not supporting OpenGL )
			tex->Sampler = sampler;

			struct Vertex
			{
				Vector3 Position;
				Vector2 UV;
			};

			// - Create a vertex layout -
			RHIVertexLayout layout =
			{
				{ "POSITION", ERHIVertexElementType::Float3 },
				{ "TEXCOORD", ERHIVertexElementType::Float2 }
			};

			// Quad
			Vertex vertices[] =
			{
				{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
				{ { 0.5f, -0.5f,  0.0f }, { 1.0f, 1.0f } },
				{ { -0.5f, 0.5f,  0.0f }, { 0.0f, 0.0f } },
				{ { 0.5f, -0.5f,  0.0f }, { 1.0f, 1.0f } },
				{ { 0.5f, 0.5f,   0.0f }, { 1.0f, 0.0f } },
				{ { -0.5f, 0.5f,  0.0f }, { 0.0f, 0.0f } }
			};

			// Cube
			Vertex cubeVerts[] =
			{
				// Front
				{ { -0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f } },
				{ { 0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
				{ { -0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f } },
				{ { 0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f } },
				{ { 0.5f, 0.5f,   0.5f }, { 1.0f, 0.0f } },
				{ { -0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f } },
				// Back
				{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
				{ { -0.5f, 0.5f,  -0.5f }, { 0.0f, 0.0f } },
				{ { 0.5f, -0.5f,  -0.5f }, { 1.0f, 1.0f } },
				{ { 0.5f, -0.5f,  -0.5f }, { 1.0f, 1.0f } },
				{ { -0.5f, 0.5f,  -0.5f }, { 0.0f, 0.0f } },
				{ { 0.5f, 0.5f,   -0.5f }, { 1.0f, 0.0f } },
				// Left
				{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
				{ { -0.5f, 0.5f,  -0.5f }, { 0.0f, 0.0f } },
				{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 1.0f } },
				{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 1.0f } },
				{ { -0.5f, 0.5f,  -0.5f }, { 0.0f, 0.0f } },
				{ { -0.5f, 0.5f,  0.5f }, { 1.0f, 0.0f } },
				// Right
				{ { 0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f } },
				{ { 0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f } },
				{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f } },
				{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f } },
				{ { 0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f } },
				{ { 0.5f, 0.5f,  -0.5f }, { 1.0f, 0.0f } },
				// Top
				{ { -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f } },
				{ { 0.5f, 0.5f,  0.5f }, { 1.0f, 1.0f } },
				{ { -0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f } },
				{ { 0.5f, 0.5f,  0.5f }, { 1.0f, 1.0f } },
				{ { 0.5f, 0.5f,  -0.5f }, { 1.0f, 0.0f } },
				{ { -0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f } },
				// Bottom
				{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
				{ { 0.5f, -0.5f,  -0.5f }, { 1.0f, 1.0f } },
				{ { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f } },
				{ { 0.5f, -0.5f,  -0.5f }, { 1.0f, 1.0f } },
				{ { 0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
				{ { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f } }
			};

			RHIVertexBufferDescriptor vbDesc;
			vbDesc.Layout = layout;
			vbDesc.InitialData = Span<const Byte>( (const Byte*)( &vertices[0] ), sizeof(vertices));
			vbDesc.Name = "My beautiful vertex buffer";

			RHIVertexBufferRef vb = RHI::CreateVertexBuffer( vbDesc );


			// - Create a Vertex Buffer -
			RHIVertexBufferDescriptor cubeVBODesc;
			cubeVBODesc.Layout = layout;
			cubeVBODesc.InitialData = Span<const uint8_t>( (const uint8_t*)( &cubeVerts[0] ), sizeof( cubeVerts ) );
			cubeVBODesc.Name = "Cube VBO";
			RHIVertexBufferRef cubeVBO = RHI::CreateVertexBuffer( cubeVBODesc );

			// HLSL Source code
			StringView vertCode = R"(
#include "Globals.hlsli"

struct InlinedConstants
{
	// Vertex Shader
	float4x4 PVM;

	// Pixel Shader
	float4 Colour;
};

INLINED_CONSTANT( inlinedConstants, InlinedConstants );

struct VSOutput
{
    float4 pos : SV_Position;  
	float2 uv : TEXCOORD;
};

VSOutput main( float3 pos : POSITION, float2 uv : TEXCOORD ) 
{
	VSOutput output;
	output.pos = mul( float4( pos, 1.0f ), inlinedConstants.PVM );
	output.uv = uv;
	return output;
}
)";

			StringView pixelCode = R"(
#include "Globals.hlsli"

struct InlinedConstants
{
	// Vertex Shader
	float4x4 PVM;

	// Pixel Shader
	float4 Colour;
};

INLINED_CONSTANT( inlinedConstants, InlinedConstants );

struct VSOutput
{
    float4 pos : SV_Position;  
	float2 uv : TEXCOORD;
};

Texture2D Texture : register( t0 );
SamplerState Sampler : register( s0 );

float4 main( VSOutput input ) : SV_Target
{
	return Texture.Sample( Sampler, input.uv ) * inlinedConstants.Colour;
}
)";

			struct InlinedConstants
			{
				float4x4 PVM;
				float4 Colour;
			};

			RHIShaderModuleRef vertShader = ShaderLibrary::LoadShader( vertCode, "My vert shader", ERHIShaderType::Vertex );
			RHIShaderModuleRef pixelShader = ShaderLibrary::LoadShader( pixelCode, "My pixel shader", ERHIShaderType::Pixel );

			// Create Shader Binding Layout
			RHIShaderBindingLayoutDescriptor sblDesc;
			sblDesc.Name = "My shader binding layout";
			sblDesc.AddBinding( "Texture"_H ).AsCombinedSamplers( 0 ).SetVisibility( ERHIShaderVisibility::All );
			sblDesc.AddBinding( "inlinedConstants"_H ).AsInlinedConstants<InlinedConstants>().SetVisibility( ERHIShaderVisibility::All );


			RHIShaderBindingLayoutRef sbl = RHI::CreateShaderBindingLayout( sblDesc );

			// - Create pipeline state -
			RHIGraphicsPipelineStateDescriptor psd;
			psd.VertexShader = vertShader;
			psd.PixelShader = pixelShader;
			psd.ColourTargetFormats[0] = ERHITextureFormat::RGBA8;
			psd.VertexLayout = layout;
			psd.ShaderBindingLayout = sbl;
			psd.RasterizerState.CullMode = ERHIRasterizerCullMode::None;
			psd.Name = "My pipeline state";
			RHIGraphicsPipelineStateRef pso = RHI::CreateGraphicsPipelineState( psd );

			RHICommandListRef cmdList = RHI::CreateCommandList( { "My beautiful command list" } );

			// - Create depth buffer -
			RHITextureDescriptor depthDesc;
			depthDesc.Width = 1280;
			depthDesc.Height = 720;
			depthDesc.Format = ERHITextureFormat::D32;
			depthDesc.Name = "My depth buffer";
			depthDesc.IsRenderTarget = true;
			depthDesc.UsageHint = ERHIUsageHint::RenderTarget;
			RHITextureRef depthTex = RHI::CreateTexture( depthDesc );

			// Temp
			float time = 0.0f;
			const Color clearColour = Color{ 0.2f, 0.35f, 0.5f, 1.0f };
			int f{};
			while ( true )
			{
#if 1
				++f;
				m_Window->OnUpdate();
				time = glfwGetTime();
				{
					RHITextureRef rt = RHI::GetSwapChain()->GetBackBuffer();

					// Resize the depth buffer to match the swap chain
					const uint32_t width = RHI::GetSwapChain()->GetWidth();
					const uint32_t height = RHI::GetSwapChain()->GetHeight();
					depthTex->Resize( width, height );

					RHIGraphicsCommandBuffer cmdBuffer;
					cmdBuffer.ResourceBarrier( rt, ERHIResourceState::Present, ERHIResourceState::RenderTarget )
						.SetRenderTargets( { &rt, 1 }, depthTex )
						.ClearRenderTargets( { &rt, 1 }, clearColour, true );

					cmdBuffer.SetGraphicsPipelineState( pso );
					cmdBuffer.SetShaderBindingLayout( sbl );

					// Set the primitive topology
					cmdBuffer.SetPrimitiveTopology( ERHITopology::Triangle );

					// Set the viewport
					Viewport vp;
					vp.Width = width;
					vp.Height = height;
					vp.X = 0;
					vp.Y = 0;
					vp.MinDepth = 0.0f;
					vp.MaxDepth = 1.0f;
					cmdBuffer.SetViewports( { &vp, 1 } );

					// Set Scissors
					ScissorRect scissor;
					scissor.Left = 0;
					scissor.Top = 0;
					scissor.Right = width;
					scissor.Bottom = height;
					cmdBuffer.SetScissors( { &scissor, 1 } );

					constexpr auto CycleRGB = +[]( float a_Time, float a_Speed = 1.0f )
						{
							float r = ( Math::Sin( a_Speed * a_Time ) + 1.0f ) / 2.0f;
							float g = ( Math::Sin( a_Speed * a_Time + 2.0f * Math::PI() / 3.0f ) + 1.0f ) / 2.0f;
							float b = ( Math::Sin( a_Speed * a_Time + 4.0f * Math::PI() / 3.0f ) + 1.0f ) / 2.0f;
							return Color( r, g, b, 1.0f );
						};

					// Get the PVM matrix
					Vector3 pos = Vector3( 0.0f, 0.0f, 3.0f );
					pos.y = Math::Sin( time ) * 2.0f;
					pos.x = Math::Cos( time ) * 2.0f;
					float4x4 model = glm::translate( Matrix4( 1.0f ), pos );
					float4x4 view = glm::lookAt( Vector3( 0.0f, 0.0f, -2.0f ), Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 0.0f, 1.0f, 0.0f ) );
					float4x4 projection = glm::perspective( Math::Radians( 90.0f ), (float)width / (float)height, 0.1f, 100.0f );

					// Construct the inlined constants
					InlinedConstants inlinedConstants;
					inlinedConstants.Colour = CycleRGB( time, 1.0f );
					inlinedConstants.PVM = projection * view * model;

					// Draw
					
					cmdBuffer.SetShaderInput( "inlinedConstants"_H, inlinedConstants );
					cmdBuffer.SetShaderInput( "Texture"_H, tex );
					cmdBuffer.SetVertexBuffer( cubeVBO );
					cmdBuffer.Draw( 0, sizeof( cubeVerts ) / sizeof( Vertex ) );

					cmdBuffer.ResourceBarrier( rt, ERHIResourceState::RenderTarget, ERHIResourceState::Present );

					cmdList->SetGraphicsCommands( cmdBuffer );

					RHI::ExecuteCommandList( cmdList );
					RHI::FenceSignal( RHI::CreateFence() );
				}
#endif
				RHI::Present();
			}
		}

		RHI::Shutdown();

		return true;

#endif

		// Initialise the Engine
		EngineConfig engineConfig;
		Engine::Singleton::Construct();
		if ( !Engine::Get()->Init( std::move( engineConfig ) ) )
		{
			return false;
		}

		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::Update()
	{
		PROFILE_FUNCTION( ProfilerCategory::Application );

		// Update Loop ========================================================================================

	#if !IS_EDITOR

		Input::SetInputMode( EInputMode::Cursor, EInputModeValue::Cursor_Disabled );

		m_ActiveScene->OnUpdate();

	#endif // IS_EDITOR

		// ====================================================================================================



		// Render Loop ========================================================================================

	#if !IS_EDITOR

		if ( !m_ActiveScene->GetMainCamera() )
		{
			auto view = m_ActiveScene->GetECS().View<CameraComponent>();
			if ( !view.empty() )
				m_ActiveScene->SetMainCamera( view.begin()[0] );
		}

		if ( m_ActiveScene->GetMainCamera() )
		{
			// Render the scene with the main camera
			Vector3 cameraPos = m_ActiveScene->GetMainCamera()->GetGameObject().GetTransform().Position;
			m_ActiveScene->GetSceneRenderer().Render(
				m_GameViewport.GetFramebuffer(),
				m_ActiveScene->GetMainCamera()->SceneCamera,
				m_ActiveScene->GetMainCamera()->GetView(),
				cameraPos );
		}
		else
		{
			// If no camera is found, render the scene with a default camera
			m_ActiveScene->GetSceneRenderer().Render(
				m_GameViewport.GetFramebuffer(),
				Camera(),
				Matrix4( 1.0f ),
				Vector3( 0.0 ) );
		}

		m_GameViewport.Resize( m_Window->GetWidth(), m_Window->GetHeight() );
		m_GameViewport.RenderToWindow();

	#endif // !IS_EDITOR


		// ====================================================================================================



		// Layers =============================================================================================

		for ( const auto& layer : m_LayerStack )
			layer->OnUpdate();

		// - ImGui -
		ImGuiLayer* imGuiLayer = Engine::Get()->m_ImGuiLayer;
		imGuiLayer->Begin();

		for ( int i = 0; i < m_LayerStack.NumLayers(); i++ )
			m_LayerStack[i]->OnImGuiDraw();

		imGuiLayer->End();
		// ---------

		// ====================================================================================================
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::Quit()
	{
	#if IS_EDITOR
		Editor::GetEditorLayer()->OnEndScene();
	#else
		Get().m_Running = false;
	#endif // IS_EDITOR
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::OnEvent( Event& a_Event )
	{
		EventDispatcher dispatcher( a_Event );
		dispatcher.Dispatch<WindowCloseEvent>( TE_BIND_EVENT_FN( Application::OnWindowClosed, 1 ) );
		dispatcher.Dispatch<WindowResizeEvent>( TE_BIND_EVENT_FN( Application::OnWindowResized, 1 ) );
	
		for ( auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			if ( a_Event.Handled )
				break;

			( *--it )->OnEvent( a_Event );
		}
	}

	bool Application::OnWindowResized( WindowResizeEvent& e )
	{
		if ( !RHI::GetSwapChain() )
			return false;

		RHI::GetSwapChain()->Resize( e.GetWidth(), e.GetHeight() );
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	bool Application::OnWindowClosed( WindowCloseEvent& e )
	{
		m_Running = false;
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::Shutdown()
	{
		Engine::Get()->Shutdown();
		Engine::Singleton::Destroy();
	}

} // namespace Tridium