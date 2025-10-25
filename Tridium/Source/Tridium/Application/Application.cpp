#include "tripch.h"
#include "Application.h"
#include <Tridium/Engine/Engine.h>

// TEMP!
#if IS_EDITOR
	#include <Tridium/Editor/Editor.h>
#endif // IS_EDITOR

// TEMP ?
#include <Tridium/ImGui/ImGuiModule.h>
#include <Tridium/Graphics/RHI/RHI.h>
#include <Tridium/Reflection/FieldReflection.h>
#include <Tridium/Asset/AssetDatabase.h>
#include <Tridium/Asset/Importers/ModelImporter.h>
#include <iostream>
#include <fstream>
#include <Tridium/Reflection/Meta.h>
#include <Tridium/Reflection/MetaAttributes.h>
#include <stb_image.h>
#include <entt/meta/meta.hpp>

namespace Tridium {

	REGISTER_TICK_GROUP( BeginTick );
	REGISTER_TICK_GROUP( BeginAppUpdate );
	REGISTER_TICK_GROUP( EndAppUpdate, "BeginAppUpdate"_H );
	REGISTER_TICK_GROUP( EndTick );

	decltype( Application::s_Instance ) Application::s_Instance = nullptr;
	decltype( Application::s_TickCallback ) Application::s_TickCallback{};

	///////////////////////////////////////////////////////////////////////////////////////////

	void Application::RequestExit( EAppExitCode a_ExitCode )
	{
		Get()->m_ExitCode = a_ExitCode;
		Get()->m_Running = false;
	}


	Application::Application( CmdLineArgs a_CmdLine )
	{
		ENSURE( !s_Instance, "An Application instance already exists!" );

		s_Instance = this;
		m_CommandLineArgs = std::move( a_CmdLine );

		// Initialise the Window
		WindowProps props;
		props.Width = 1280;
		props.Height = 720;
		m_Window = Window::Create( props );
		m_Window->SetEventCallback( [ this ]( const Event& a_Event ) { this->EnqueueEvent( a_Event ); } );

		// Initialise the Engine
		EngineConfig engineConfig;
		m_Engine = Engine::Create( engineConfig );

	#if WITH_EDITOR
		m_LayerStack.EmplaceOverlay<Editor>();
	#endif // WITH_EDITOR

	}

	///////////////////////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
		s_Instance = nullptr;
	}
	
	EAppExitCode Application::Run()
	{
		m_Running = true;

		uint32_t frameCounter = 0;
		double fpsInterval = 0.0;
		uint32_t minFPS = 0xFFFFFFFF;
		uint32_t maxFPS = 0;

		SetUpTickGroups();

		while ( m_Running )
		{
			PROFILE_FRAME();

			const double lastFrameTime = Time::Now();
			Time::Update();

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

			FlushEventQueue();

			m_LayerStack.OnUpdate( Cast<float>( Time::DeltaTime() ) );

			// Invoke the tick groups
			s_TickCallback.Broadcast();

			m_Window->OnUpdate();
		}

		TODO( "Probably remove layers" );
		m_LayerStack = {};

		m_Engine.reset();
		return m_ExitCode;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////
	void Application::FlushEventQueue()
	{
		while ( !m_EventQueue.empty() )
		{
			Event& event = m_EventQueue.front();

			EventDispatcher dispatcher( event );
			dispatcher.Dispatch<WindowCloseEvent>( [this]( const WindowCloseEvent& a_Event ) -> bool { return OnWindowClosed( a_Event ); } );
			dispatcher.Dispatch<WindowResizeEvent>( [this]( const WindowResizeEvent& a_Event ) -> bool { return OnWindowResized( a_Event ); } );

			m_LayerStack.OnEvent( event );

			m_EventQueue.pop();
		}
	}

	void Application::SetUpTickGroups()
	{
		s_TickCallback.Clear();

		struct TickGroupNode
		{
			bool Registered = false;
			HashedString Name;
			Array< TickGroupNode* > Dependents;
		};

		Map< HashedString, TickGroupNode > tickGroupNodes;
		Queue< TickGroupNode* > queue;

		// Add in BeginTick
		for ( const auto& tickFn : GetTickGroups()[ TickGroups::BeginTick ].Callbacks )
		{
			s_TickCallback.Add( tickFn );
		}

	#define LOG_TICK_GROUP( _Name ) LOG( LogCategory::Application, Info, "	- {}", _Name )

		LOG( LogCategory::Application, Info, "Setting up Tick Groups..." );
		// Log tick group order

		LOG( LogCategory::Application, Info, "Tick Group Order:" );
		LOG_TICK_GROUP( "BeginTick" );

		for ( const auto& [name, tickGroup] : GetTickGroups() )
		{
			if ( name == TickGroups::BeginTick || name == TickGroups::EndTick )
			{
				continue;
			}

			TickGroupNode& node = tickGroupNodes[ name ];

			node.Registered = true;
			node.Name = name;

			for ( const HashedString& Prereq : tickGroup.Prerequisites )
			{
				TickGroupNode& PrereqNode = tickGroupNodes[ Prereq ];

				PrereqNode.Registered = false;
				PrereqNode.Dependents.EmplaceBack( &node );
			}

			// If there are no prereqs, add to queue.
			// We want tick groups with no prerequisites to fire first.
			if ( tickGroup.Prerequisites.Empty() )
			{
				queue.push( &node );
			}
		}

		while ( !queue.empty() )
		{
			TickGroupNode* node = queue.front();
			queue.pop();

			LOG_TICK_GROUP( node->Name.String() );

			// We want to add to the delegate in the order of the tick groups.
			const auto it = GetTickGroups().find( node->Name );

			for ( const auto& tickFn : it->second.Callbacks )
			{
				s_TickCallback.Add( tickFn );
			}

			for ( TickGroupNode* Dependent : node->Dependents )
			{
				queue.push( Dependent );
			}
		}

		// Add in EndTick
		for ( const auto& tickFn : GetTickGroups()[ TickGroups::EndTick ].Callbacks )
		{
			s_TickCallback.Add( tickFn );
		}

		LOG_TICK_GROUP( "EndTick" );
	}

	bool Application::OnWindowResized( const WindowResizeEvent& a_Event )
	{
		if ( !RHI::GetDynamicRHI()->GetSwapChain() )
			return false;

		RHI::GetDynamicRHI()->GetSwapChain()->Resize( a_Event.Width, a_Event.Height );
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	bool Application::OnWindowClosed( const WindowCloseEvent& a_Event )
	{
		m_Running = false;
		return true;
	}

	UnorderedMap<HashedString, Application::TickGroup>& Application::GetTickGroups()
	{
		static UnorderedMap<HashedString, TickGroup> s_TickGroups{};
		return s_TickGroups;
	}

#if 0

	bool Application::Init()
	{
		// TEMP!
#if 1
		UniquePtr<IEngineModule> rendererModule = MakeUnique<RendererModule>();
		auto bleh = rendererModule->Init();

		{
			if ( auto error = T::AssetDatabase::Init(); error.IsError() )
			{
				LOG( LogCategory::Asset, Error, "Failed to initialise the Asset Database: {0}", error.Error() );
				return false;
			}
			else
			{
				LOG( LogCategory::Asset, Info, "Asset Database initialised successfully" );
			}


			uint8_t testImgData[ 64 * 64 * 4 ];
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
						testImgData[ ( y * 64 + x ) * 4 + 0 ] = 255;
						testImgData[ ( y * 64 + x ) * 4 + 1 ] = 0;
						testImgData[ ( y * 64 + x ) * 4 + 2 ] = 0;
						testImgData[ ( y * 64 + x ) * 4 + 3 ] = 255;
					}
					else if ( y < 64 / 3 * 2 )
					{
						testImgData[ ( y * 64 + x ) * 4 + 0 ] = 0;
						testImgData[ ( y * 64 + x ) * 4 + 1 ] = 255;
						testImgData[ ( y * 64 + x ) * 4 + 2 ] = 0;
						testImgData[ ( y * 64 + x ) * 4 + 3 ] = 255;
					}
					else
					{
						testImgData[ ( y * 64 + x ) * 4 + 0 ] = 0;
						testImgData[ ( y * 64 + x ) * 4 + 1 ] = 0;
						testImgData[ ( y * 64 + x ) * 4 + 2 ] = 255;
						testImgData[ ( y * 64 + x ) * 4 + 3 ] = 255;
					}
				}
			}

			RHITextureSubresourceData testImgSubresData;
			testImgSubresData.Data = testImgData;
			testImgSubresData.RowStride = 64 * 4; // 4 bytes per pixel

			// - Create a sampler -
			RHISampler sampler;
			sampler.AddressU = ERHISamplerAddressMode::Clamp;
			sampler.AddressV = ERHISamplerAddressMode::Clamp;
			sampler.AddressW = ERHISamplerAddressMode::Clamp;

			// - Create a texture -
			auto texDesc = RHITextureDesc{}.SetWidth( 64 )
				.SetHeight( 64 )
				.SetFormat( ERHIFormat::RGBA8_UNORM )
				.SetDimension( ERHITextureDimension::Texture2D )
				.SetDefaultSampler( sampler )
				.SetName( "My texture" );
			//auto texAsset = Texture::Create( testImgData, texDesc );
			//texAsset->ClearPixelData();
			//T::AssetMetadata texAssetMetadata = T::AssetMetadata::From( *texAsset );
			//texAssetMetadata.Name = texDesc.Name;
			//T::AssetDatabase::RegisterAsset( texAsset.get(), std::move( texAssetMetadata ) );
			RHITextureSubresourceData subresourceData;
			subresourceData.Data = testImgData;
			subresourceData.RowStride = 64 * 4; // 4 bytes per pixel
			subresourceData.DepthStride = 0; // Not a 3D texture, so depth stride is not used

			RHITextureRef tex = RHI::CreateTexture( texDesc, testImgSubresData );

			AssetID lionTexID;
			{
				auto lionTexEx = Texture::Load( "6772804448157695701.jpg" );
				if ( lionTexEx.IsError() )
				{
					LOG( LogCategory::Asset, Error, "Failed to load lion texture: {0}", lionTexEx.Error() );
					return false;
				}

				SharedPtr<Texture> lionTex = lionTexEx.Value();
				T::AssetDatabase::RegisterAsset( lionTex, T::AssetMetadata::From( *lionTex ) );
				lionTexID = lionTex->ID();

				RHITextureDesc lionTexDesc = RHITextureDesc{}
					.SetWidth( lionTex->Width() )
					.SetHeight( lionTex->Height() )
					.SetFormat( lionTex->Format() )
					.SetDimension( lionTex->Dimension() )
					.SetName( "Lion Texture" );

				RHITextureSubresourceData lionTexSubresData;
				lionTexSubresData.Data = lionTex->PixelData().Data();
				lionTexSubresData.RowStride = lionTex->Width() * GetRHIFormatInfo( lionTex->Format() ).Bytes();
				lionTexSubresData.DepthStride = 0; // Not a 3D texture, so depth stride is not used
				RHITextureRef lionTexRHI = RHI::CreateTexture( lionTexDesc, lionTexSubresData );

				RenderResourceTexture lionTexResource;
				lionTexResource.AssetID = lionTex->ID();
				lionTexResource.Texture = lionTexRHI;

				RenderResourceManager::Get()->AddTexture( lionTexResource );
			}

			struct Vertex
			{
				Vector3 Position;
				Vector2 UV;
				Vector3 Normal;
			};
			constexpr RHIVertexLayout layout = RHIVertexLayout::From<Vertex>();

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
				{ { -0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
				{ { 0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
				{ { -0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
				{ { 0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
				{ { 0.5f, 0.5f,   0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
				{ { -0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
				// Back
				{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
				{ { -0.5f, 0.5f,  -0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
				{ { 0.5f, -0.5f,  -0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
				{ { 0.5f, -0.5f,  -0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
				{ { -0.5f, 0.5f,  -0.5f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
				{ { 0.5f, 0.5f,   -0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
				// Left
				{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },
				{ { -0.5f, 0.5f,  -0.5f }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
				{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 1.0f } , { -1.0f, 0.0f, 0.0f } },
				{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 1.0f } , { -1.0f, 0.0f, 0.0f } },
				{ { -0.5f, 0.5f,  -0.5f }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },
				{ { -0.5f, 0.5f,  0.5f }, { 1.0f, 0.0f } , { -1.0f, 0.0f, 0.0f } },
				// Right
				{ { 0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f } , { 1.0f, 0.0f, 0.0f } },
				{ { 0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f } , { 1.0f, 0.0f, 0.0f } },
				{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
				{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
				{ { 0.5f, 0.5f,  0.5f }, { 0.0f, 0.0f } , { 1.0f, 0.0f, 0.0f } },
				{ { 0.5f, 0.5f,  -0.5f }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
				// Top
				{ { -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f } , { 0.0f, 1.0f, 0.0f } },
				{ { 0.5f, 0.5f,  0.5f }, { 1.0f, 1.0f } , { 0.0f, 1.0f, 0.0f } },
				{ { -0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
				{ { 0.5f, 0.5f,  0.5f }, { 1.0f, 1.0f } , { 0.0f, 1.0f, 0.0f } },
				{ { 0.5f, 0.5f,  -0.5f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
				{ { -0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
				// Bottom
				{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } , { 0.0f, -1.0f, 0.0f }},
				{ { 0.5f, -0.5f,  -0.5f }, { 1.0f, 1.0f } , { 0.0f, -1.0f, 0.0f }},
				{ { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f }  , { 0.0f, -1.0f, 0.0f }},
				{ { 0.5f, -0.5f,  -0.5f }, { 1.0f, 1.0f } , { 0.0f, -1.0f, 0.0f }},
				{ { 0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f }  , { 0.0f, -1.0f, 0.0f }},
				{ { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f }  , { 0.0f, -1.0f, 0.0f }}
			};

			// - Create a Vertex Buffer -
			RHIBufferDesc cubeVBODesc;
			cubeVBODesc.Name = "Cube VBO";
			cubeVBODesc.BindFlags = ERHIBindFlags::VertexBuffer;
			cubeVBODesc.Size = sizeof( cubeVerts );
			//cubeVBODesc.Stride = sizeof( Vertex );
			RHIBufferRef cubeVBO = RHI::CreateBuffer( cubeVBODesc, Span<uint8_t>{ reinterpret_cast< uint8_t* >( cubeVerts ), sizeof( cubeVerts ) } );



			// HLSL Source code
			StringView vertCode = R"(
#include "Globals.hlsli"

struct Light
{
	float4 Colour;
	float3 Position;
	float Intensity;
};

struct InlinedConstants
{
	float4x4 PVM;
	float4x4 Model;
};

INLINED_CONSTANTS( inlinedConstants, InlinedConstants );

struct Constants
{
	Light LightData;
};

CONSTANT_BUFFER( constants, Constants, 1 );

struct Vertex
{
	float3 Position : Position;
	float2 UV : UV;
	float3 Normal : Normal;
};

struct VSOutput
{
    float4 pos : SV_Position;  
	float3 worldPos : WORLD_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

VSOutput VSMain( Vertex a_Vertex ) 
{
	VSOutput output;
	output.worldPos = mul( float4( a_Vertex.Position, 1.0f ), inlinedConstants.Model ).xyz;
	output.pos = mul( float4( a_Vertex.Position, 1.0f ), inlinedConstants.PVM );
	output.uv = a_Vertex.UV;
	output.normal = a_Vertex.Normal;
	return output;
}
)";

			StringView pixelCode = R"(
#include "Globals.hlsli"

struct Light
{
	float4 Colour;
	float3 Position;
	float Intensity;
};

struct InlinedConstants
{
	// Vertex Shader
	float4x4 PVM;
	float4x4 Model;
};

INLINED_CONSTANTS( inlinedConstants, InlinedConstants );

struct Constants
{
	Light LightData;
};

CONSTANT_BUFFER( constants, Constants, 1 );

struct VSOutput
{
    float4 pos : SV_Position;  
	float3 worldPos : WORLD_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

//COMBINED_SAMPLER( Texture, Texture2D, 0 );

Texture2D Texture : register( t0 );
SamplerState TextureSampler : register( s0 );

float4 PSMain( VSOutput input ) : SV_Target
{
	// Simple Phong Lighting
	float3 normal = normalize( input.normal );
	float3 lightDir = normalize( constants.LightData.Position - input.worldPos );
	float3 viewDir = normalize( float3( 0.0f, 0.0f, -1.0f ) - input.worldPos );
	float3 reflectDir = reflect( -lightDir, normal );
	float3 ambient = 0.1f * constants.LightData.Colour.rgb + 0.2f * Texture.Sample( TextureSampler, input.uv ).rgb;
	float3 diffuse = max( dot( normal, lightDir ), 0.0f ) * constants.LightData.Colour.rgb;
	float3 specular = 1000.0f * pow( max( dot( viewDir, reflectDir ), 0.0f ), 32.0f ) * constants.LightData.Colour.rgb;
	float3 color = ambient + diffuse + specular;
	color *= constants.LightData.Intensity;
	//color = Texture.Sample( TextureSampler, input.uv ).rgb;
	return float4( color, 1.0f );
}
)";

			struct Light
			{
				float4 Colour;
				float3 Position;
				float Intensity;
			};

			struct InlinedConstants
			{
				float4x4 PVM;
				float4x4 Model;
			};

			struct Constants
			{
				Light LightData;
			};

			RHIShaderModuleRef vertShader = ShaderLibrary::Get()->LoadShader( vertCode, "My vert shader", ERHIShaderType::Vertex );
			RHIShaderModuleRef pixelShader = ShaderLibrary::Get()->LoadShader( pixelCode, "My pixel shader", ERHIShaderType::Pixel );

			// Create Shader Binding Layout
			RHIBindingLayoutDesc sblDesc;
			sblDesc.Name = "My shader binding layout";
			sblDesc.Visibility = ERHIShaderVisibility::All;
			sblDesc.AddBinding( "inlinedConstants"_H ).AsInlinedConstants( 0, 128 );
			sblDesc.AddBinding( "constants"_H ).AsConstantBuffer( 1 );
			sblDesc.AddBinding( "Texture"_H ).AsTexture( 0 );
			RHIBindingLayoutRef sbl = RHI::CreateBindingLayout( sblDesc );

			RHIFramebufferInfo fbInfo{};
			fbInfo.SetColorFormats( { ERHIFormat::RGBA8_UNORM } );
			fbInfo.SetDepthStencilFormat( ERHIFormat::D32_FLOAT );

			// - Create pipeline state -
			RHIGraphicsPipelineStateDesc psd{};
			psd.Topology = ERHITopology::Triangle;
			psd.VertexShader = vertShader;
			psd.PixelShader = pixelShader;
			psd.FramebufferInfo = fbInfo;
			psd.VertexLayout = layout;
			psd.BindingLayouts.EmplaceBack( sbl );
			psd.RasterizerState.CullMode = ERHICullMode::None;
			psd.RasterizerState.AnitaliasedLinesEnabled = false;
			//psd.RasterizerState.FillMode = ERHIFillMode::Wireframe;
			psd.DepthState.DepthTestEnabled = true;
			psd.DepthState.DepthWriteEnabled = true;
			psd.Name = "My pipeline state";
			RHIGraphicsPipelineStateRef pso = RHI::CreateGraphicsPipelineState( psd );

			// - Create depth buffer -
			RHITextureDesc depthDesc;
			depthDesc.Width = 1280;
			depthDesc.Height = 720;
			depthDesc.Format = ERHIFormat::D32_FLOAT;
			depthDesc.Name = "My depth buffer";
			depthDesc.BindFlags = ERHIBindFlags::DepthStencil;
			depthDesc.Dimension = ERHITextureDimension::Texture2D;
			depthDesc.UseClearValue = true;
			RHITextureRef depthTex = RHI::CreateTexture( depthDesc );

			RHICommandListDesc cmdListDesc;
			cmdListDesc.QueueType = ERHICommandQueueType::Graphics;
			RHICommandListRef cmdList = RHI::CreateCommandList( cmdListDesc );

			// Temp
			float time = 0.0f;
			const Color4 clearColor = Color4{ 0.2f, 0.35f, 0.5f, 1.0f };
			const RHIClearValue clearValue{ clearColor };
			int f{};

			cmdList->Open();
			auto dstSlice = RHITextureSlice{}.SetOffset( 30, 30 ).SetSize( 200, 200 );
			auto srcSlice = RHITextureSlice::EntireTexture().SetSize( 34, 34 );
			cmdList->CopyTexture(
				*RenderResourceManager::Get()->GetTexture( lionTexID ).Texture, dstSlice,
				*tex, srcSlice
			);
			cmdList->Close();
			IRHICommandList* cmdListPtr = cmdList.get();
			RHI::ExecuteCommandLists( &cmdListPtr, 1, ERHICommandQueueType::Graphics );

			//while ( time < 5.0 )

			int bb = 0;
			while ( true )
			{
#if 1
				++f;
				m_Window->OnUpdate();
				FlushEventQueue();
				time = glfwGetTime();

				RHI::BeginFrame();

				{
					cmdList->Open();

					RHIGraphicsState graphicsState{};

					RHITextureRef rt = RHI::GetSwapChain()->GetBackBuffer();
					if ( !rt )
						continue;

					// Resize the depth buffer to match the swap chain
					const auto rtDesc = rt->Desc();
					const uint32_t width = rtDesc.Width;
					const uint32_t height = rtDesc.Height;
					if ( width == 0 || height == 0 )
						continue;

					const auto& depthDesc = depthTex->Desc();
					if ( width != depthDesc.Width || height != depthDesc.Height )
					{
						// Create new depth texture
						RHITextureDesc newDepthDesc = depthDesc;
						newDepthDesc.Width = width;
						newDepthDesc.Height = height;

						depthTex = RHI::CreateTexture( newDepthDesc );
					}

					constexpr auto CycleRGB = +[]( float a_Time, float a_Speed = 1.0f ) {
						float r = ( Math::Sin( a_Speed * a_Time ) + 1.0f ) / 2.0f;
						float g = ( Math::Sin( a_Speed * a_Time + 2.0f * Math::PI() / 3.0f ) + 1.0f ) / 2.0f;
						float b = ( Math::Sin( a_Speed * a_Time + 4.0f * Math::PI() / 3.0f ) + 1.0f ) / 2.0f;
						return Color4( r, g, b, 1.0f );
						};

					Constants constants{};
					Light light;
					light.Colour = Color4( 1, 0, 0, 1 );
					light.Position = Vector3( 0.0f, 0.0f, 2.0f );
					light.Intensity = 1;
					constants.LightData = light;

					RHIBufferDesc constantsDesc{
						"constants buffer",
						256,
						ERHIBindFlags::ConstantBuffer
					};

					RHIBufferRef constantsBuffer = RHI::CreateBuffer(
						constantsDesc, Span{ ReinterpretCast<uint8_t*>( &constants ), sizeof( Constants ) }
					);

					RHIBindingSetDesc bindingSetDesc{ sbl };
					//bindingSetDesc.AddCombinedSampler( "Texture"_H, *tex );
					bindingSetDesc.AddConstantBuffer( "constants"_H, constantsBuffer.get() );
					auto sampler = RHISampler{}.SetAddressU( ERHISamplerAddressMode::Border )
						.SetAddressV( ERHISamplerAddressMode::Border )
						.SetAddressW( ERHISamplerAddressMode::Border )
						.SetFilter( ERHISamplerFilter::Anisotropic )
						.SetMaxAnisotropy( 16 )
						.SetBorderColor( Color4( 1, 1, 0, 1.0f ) );

					bindingSetDesc.AddTexture( "Texture"_H, RenderResourceManager::Get()->GetTexture( lionTexID ).Texture.get(), &sampler );
					RHIBindingSetRef bindingSet = RHI::CreateBindingSet( bindingSetDesc );

					graphicsState.PipelineState = pso.get();
					graphicsState.AddBindingSet( bindingSet.get() );
					graphicsState.VertexBuffer = cubeVBO.get();
					graphicsState.Framebuffer
						.AddColorAttachment( rt.get() )
						.SetDepthStencilAttachment( depthTex.get() );

					cmdList->SetGraphicsState( graphicsState );

					cmdList->ClearRenderTargets( ERHIClearFlags::All, clearValue );

					// Set the viewport
					RHIViewportState viewportState{};
					viewportState.AddViewportAndScissor( RHIViewport( 0, 0, width, height ) );
					cmdList->SetViewportState( viewportState );

					// Get the PVM matrix
					Vector3 pos = Vector3( 0.0f, 0.0f, 3.0f );
					pos.Y = Math::Sin( time ) * 2.0f;
					pos.X = Math::Cos( time ) * 2.0f;
					float4x4 model = Math::Translate( pos );
					float4x4 view = Math::LookAt( Vector3( 0.0f, 0.0f, -2.0f ), Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 0.0f, 1.0f, 0.0f ) );
					float4x4 projection = Math::Perspective( Math::Radians( 90.0f ), ( float )width / ( float )height, 0.1f, 100.0f );

					// Construct the inlined constants ( random struct thats casted to an array of bytes )
					InlinedConstants inlinedConstants;
					inlinedConstants.PVM = projection * view * model;
					inlinedConstants.Model = model;

					cmdList->PushDebugGroup( "Draw Cube 1" );
					cmdList->InsertDebugMarker( "Debug Marker" );
					cmdList->SetInlinedConstants( inlinedConstants );

					RHIDrawArgs drawArgs{};
					drawArgs.BaseVertex = 0;
					drawArgs.VertexCount = sizeof( cubeVerts ) / sizeof( Vertex );
					cmdList->Draw( drawArgs );

					cmdList->PopDebugGroup();

					cmdList->PushDebugGroup( "Draw Cube 2" );

					inlinedConstants.Model = Math::Translate( Vector3( 0.5 * -pos.X, -pos.Y, -pos.X + pos.Z ) );
					inlinedConstants.PVM = projection * view * inlinedConstants.Model;
					cmdList->SetInlinedConstants( inlinedConstants );

					constants.LightData.Colour = Color4( 0, 1, 0, 1 );
					cmdList->UpdateBuffer( *constantsBuffer, ReinterpretCast<const void*>( &constants ), sizeof( constants ) );
					cmdList->Draw( drawArgs );

					cmdList->PopDebugGroup();

					cmdList->ResourceBarrier( *rt, ERHIResourceStates::Present );
					cmdList->ResourceBarrier( *depthTex, ERHIResourceStates::Present );

					cmdList->Close();


					IRHICommandList* cmdListPtr = cmdList.get();
					RHI::ExecuteCommandLists( &cmdListPtr, 1, ERHICommandQueueType::Graphics );
					RHI::WaitForIdle();

					RHI::CollectGarbage();
				}
#endif
				RHI::Present();

				RHI::EndFrame();
			}
		}

		rendererModule->Shutdown();

		return false;

#endif

		return true;
	}

#endif

} // namespace Tridium