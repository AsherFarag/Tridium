#include "tripch.h"
#include "ImGuiBackend_RHI.h"

#include <stdint.h>     // intptr_t

// Clang warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"    // warning: implicit conversion changes signedness
#endif

#include <Tridium/Graphics/RHI/RHIShaderCompiler.h>
#include <Tridium/Graphics/RHI/RHI.h>

namespace Tridium {

    struct InlinedConstants
    {
        Matrix4 ProjectionMatrix;
	};

	static constexpr size_t c_MaxCachedBindingSets = 1;

    static const char* s_ImGuiVertShader = R"(
    #include "Core.hlsli"

    struct InlinedConstants
    {
        float4x4 PVM;
    };

    INLINED_CONSTANTS( inlinedConstants, InlinedConstants );

    struct VS_INPUT
    {
       float2 pos : POSITION;
       float2 uv  : TEXCOORD0;
       float4 col  : COLOR0;
    };

    struct PS_INPUT
    {
      float4 pos : SV_POSITION;
      float4 col : COLOR0;
      float2 uv  : TEXCOORD0;
    };

    PS_INPUT VSMain(VS_INPUT input)
    {
      PS_INPUT output;
      output.pos = mul(inlinedConstants.PVM, float4(input.pos.xy, 0.0f, 1.0f));
      output.uv  = input.uv;
      output.col = input.col;
      return output;
    } )";

    static const char* s_ImGuiPixelShader = R"(
    #include "Core.hlsli"

    struct PS_INPUT
    {
      float4 pos : SV_POSITION;
      float4 col : COLOR0;
      float2 uv  : TEXCOORD0;
    };
    COMBINED_SAMPLER( Texture, Texture2D, 0 );
    
    float4 PSMain(PS_INPUT input) : SV_Target
    {
      return input.col * SampleTexture( Texture, input.uv );
    } )";

    static const char* s_ImGuiDepthTexturePixelShader = R"(
    #include "Core.hlsli"

    struct PS_INPUT
    {
      float4 pos : SV_POSITION;
      float4 col : COLOR0;
      float2 uv  : TEXCOORD0;
    };
    COMBINED_SAMPLER( Texture, Texture2D, 0 );
    
    float4 PSMain(PS_INPUT input) : SV_Target
    {
        float depth = SampleTexture( Texture, input.uv ).r;
        // Approximate linearization — adjust k to taste
        const float k = 0.2; // smaller = more contrast near camera, larger = flatter
        float linearized = pow(depth, k);

        // Visualize it as grayscale
        return float4(linearized.xxx, 1.0);
    } )";

    // RHI data
    struct ImGui_ImplRHI_Data
    {
        IDynamicRHI* DynamicRHI = nullptr;
        RHIBufferRef VertexBuffer = nullptr;
        RHIBufferRef IndexBuffer = nullptr;
        RHITextureRef FontTexture = nullptr;
        RHICommandListRef CommandList = nullptr;
        RHIBindingLayoutRef BindingLayout = nullptr;
        RHIGraphicsPipelineStateRef PipelineState = nullptr;
		RHIGraphicsPipelineStateRef DepthTexturePipelineState = nullptr;
        UnorderedMap<IRHITexture*, RHIBindingSetRef> BindingSetsCache;

        const RHIBindingSetRef& GetOrCreateBindingSet( IRHITexture* a_Texture )
        {
            auto it = BindingSetsCache.find( a_Texture );
            if ( it != BindingSetsCache.end() )
                return it->second;

            static constexpr auto sampler = RHISampler{}
                .SetFilter( ERHISamplerFilter::MinMagMipLinear )
                .SetAddressU( ERHISamplerAddressMode::Repeat )
                .SetAddressV( ERHISamplerAddressMode::Repeat )
                .SetAddressW( ERHISamplerAddressMode::Repeat )
                .SetMipLODBias( 0.0f )
                .SetMaxAnisotropy( 0 )
                .SetComparisonFunc( ERHIComparison::Never )
                .SetBorderColor( Color4::Black() )
                .SetMinLOD( 0.0f )
                .SetMaxLOD( 0.0f );

            const auto desc = RHIBindingSetDesc{}
                .SetLayout( BindingLayout )
                .AddTexture( "Texture"_H, a_Texture, &sampler )
                .SetName( "ImGui Binding Set" );

            return BindingSetsCache[ a_Texture ] = DynamicRHI->CreateBindingSet( desc );
        }

        ImGui_ImplRHI_Data() = default;
    };

    // Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
    // It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
    static ImGui_ImplRHI_Data* ImGui_ImplRHI_GetBackendData()
    {
        return ImGui::GetCurrentContext() ? ReinterpretCast<ImGui_ImplRHI_Data*>( ImGui::GetIO().BackendRendererUserData ) : nullptr;
    }

    // Functions
    bool ImGui_ImplRHI_Init( IDynamicRHI* a_RHI )
    {
        ImGuiIO& io = ImGui::GetIO();
        IMGUI_CHECKVERSION();
        IM_ASSERT( io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!" );
        IM_ASSERT( a_RHI != nullptr && "RHI not initialized!" );

        // Setup backend capabilities flags
        ImGui_ImplRHI_Data* bd = IM_NEW( ImGui_ImplRHI_Data )( );
        io.BackendRendererUserData = ( void* )bd;
        io.BackendRendererName = "imgui_impl_rhi";
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.

        bd->DynamicRHI = a_RHI;

        return true;
    }

    void ImGui_ImplRHI_Shutdown()
    {
        ImGui_ImplRHI_Data* bd = ImGui_ImplRHI_GetBackendData();
        IM_ASSERT( bd != nullptr && "No renderer backend to shutdown, or already shutdown?" );
        ImGuiIO& io = ImGui::GetIO();

        ImGui_ImplRHI_DestroyDeviceObjects();

        io.BackendRendererName = nullptr;
        io.BackendRendererUserData = nullptr;
        io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;
        IM_DELETE( bd );
    }

    static void ImGui_ImplRHI_SetupRenderState( IDynamicRHI* a_RHI )
    {
    }

    void ImGui_ImplRHI_NewFrame()
    {
        ImGui_ImplRHI_Data* bd = ImGui_ImplRHI_GetBackendData();
        IM_ASSERT( bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplRHI_Init()?" );

        if ( !bd->FontTexture )
            ImGui_ImplRHI_CreateDeviceObjects();

		// Clear binding sets cache every frame to avoid keeping stale references to textures
        if ( bd->BindingSetsCache.size() > c_MaxCachedBindingSets )
        {
            bd->BindingSetsCache.clear();
        }
    }

	static bool ImGui_ImplRHI_ReallocateBuffer( RHIBufferRef& a_Buffer, size_t a_ReqSize, size_t a_ReallocSize, bool a_IsIndexBuffer )
    {
        if ( a_Buffer == nullptr || !a_Buffer->Valid() || a_Buffer->Desc().Size < a_ReqSize )
        {
            auto desc = RHIBufferDesc{}
				.SetName( a_IsIndexBuffer ? "ImGui Index Buffer" : "ImGui Vertex Buffer" )
                .SetSize( a_ReallocSize )
                .SetHeapType( ERHIHeapType::Dynamic )
				.SetBindFlags( a_IsIndexBuffer ? ERHIBindFlags::IndexBuffer : ERHIBindFlags::VertexBuffer );

            if ( a_IsIndexBuffer )
            {
                desc.SetType( ERHIBufferType::Formatted )
                    .SetFormat( GetRHIFormatFromType<ImDrawIdx>() )
                    .SetStride( sizeof( ImDrawIdx ) );
            }
            else
            {
                desc.SetType( ERHIBufferType::Structured )
                    .SetStride( sizeof( ImDrawVert ) );
            }

			a_Buffer = RHI::CreateBuffer( desc );
        }

        return a_Buffer != nullptr && a_Buffer->Valid();
	}

    static bool ImGui_ImplRHI_UpdateGeometry( ImDrawData* a_DrawData, const RHICommandListRef& a_CommandList )
    {
		ImGui_ImplRHI_Data* bd = ImGui_ImplRHI_GetBackendData();

        if ( !ImGui_ImplRHI_ReallocateBuffer( bd->VertexBuffer, a_DrawData->TotalVtxCount * sizeof( ImDrawVert ),
                                              ( a_DrawData->TotalVtxCount + 5000 ) * sizeof( ImDrawVert ),
                                              false ) )
        {
            return false;
        }

        if ( !ImGui_ImplRHI_ReallocateBuffer( bd->IndexBuffer, a_DrawData->TotalIdxCount * sizeof( ImDrawIdx ),
                                              ( a_DrawData->TotalIdxCount + 5000 ) * sizeof( ImDrawIdx ),
                                              true ) )
        {
            return false;
		}

        if ( a_DrawData->TotalVtxCount == 0 || a_DrawData->TotalIdxCount == 0 )
			return true;

		static Array<ImDrawVert> s_VertexBufferData; s_VertexBufferData.Resize( a_DrawData->TotalVtxCount );
		static Array<ImDrawIdx> s_IndexBufferData; s_IndexBufferData.Resize( a_DrawData->TotalIdxCount );

        // copy and convert all vertices into a single contiguous buffer
        ImDrawVert* vtxDst = &s_VertexBufferData[ 0 ];
        ImDrawIdx* idxDst = &s_IndexBufferData[ 0 ];
        for ( int n = 0; n < a_DrawData->CmdListsCount; n++ )
        {
            const ImDrawList* cmdList = a_DrawData->CmdLists[ n ];
            memcpy( vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof( ImDrawVert ) );
            memcpy( idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof( ImDrawIdx ) );
            vtxDst += cmdList->VtxBuffer.Size;
            idxDst += cmdList->IdxBuffer.Size;
        }

		a_CommandList->UpdateBuffer( *bd->VertexBuffer, s_VertexBufferData.Data(), s_VertexBufferData.Size() * sizeof( ImDrawVert ) );
        a_CommandList->UpdateBuffer( *bd->IndexBuffer, s_IndexBufferData.Data(), s_IndexBufferData.Size() * sizeof( ImDrawIdx ) );

		return true;
    }

    void ImGui_ImplRHI_RenderDrawData( ImDrawData* a_DrawData, IDynamicRHI* a_RHI, RHITextureRef a_RenderTarget )
    {
        const ImVec2 renderScale{ a_DrawData->FramebufferScale.x, a_DrawData->FramebufferScale.y };

        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        const int fbWidth = ( int )( a_DrawData->DisplaySize.x * renderScale.x );
        const int fbHeight = ( int )( a_DrawData->DisplaySize.y * renderScale.y );
        if ( fbWidth == 0 || fbHeight == 0 )
            return;

        // Will project scissor/clipping rectangles into framebuffer space
        ImVec2 clipOff = a_DrawData->DisplayPos;         // (0,0) unless using multi-viewports
        ImVec2 clipScale = renderScale;

        // Render command lists
        ImGui_ImplRHI_SetupRenderState( a_RHI );

        ImGui_ImplRHI_Data* bd = ImGui_ImplRHI_GetBackendData();

        const RHICommandListRef& cmdList = bd->CommandList;
        cmdList->Open();
		cmdList->PushDebugGroup( "ImGui Render" );

        if ( !ImGui_ImplRHI_UpdateGeometry( a_DrawData, cmdList ) )
        {
			ASSERT( false, "Failed to update geometry for ImGui rendering!" );
			cmdList->Close();
            return;
        }



        const InlinedConstants inlinedConstants = [&]()
        {
            const float L = a_DrawData->DisplayPos.x;
            const float R = a_DrawData->DisplayPos.x + a_DrawData->DisplaySize.x;
            const float T = a_DrawData->DisplayPos.y;
            const float B = a_DrawData->DisplayPos.y + a_DrawData->DisplaySize.y;

            const Matrix4 result = Matrix4{
                {  2.0f / ( R - L ),   0.0f,              0.0f,    ( R + L ) / ( L - R ) },
                {  0.0f,               2.0f / ( T - B ),  0.0f,    ( T + B ) / ( B - T ) },
                {  0.0f,               0.0f,              0.5f,    0.5f                  },
                {  0.0f,               0.0f,              0.0f,    1.0f                  }
            };

			return InlinedConstants{ .ProjectionMatrix = result };
		}();

        auto graphicsState = RHIGraphicsState{}
            .SetPipelineState( bd->PipelineState.get() )
			.SetVertexBuffer( bd->VertexBuffer.get() )
			.SetIndexBuffer( bd->IndexBuffer.get() )
			.SetFramebuffer( RHIFramebuffer{}.AddColorAttachment( a_RenderTarget ) );

		graphicsState.BindingSets.Resize( 1 );

		size_t vtxOffset = 0;
		size_t idxOffset = 0;
        for ( int n = 0; n < a_DrawData->CmdListsCount; n++ )
        {
            const ImDrawList* cmd_list = a_DrawData->CmdLists[ n ];

            for ( int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++ )
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[ cmd_i ];
                if ( pcmd->UserCallback )
                {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                    if ( pcmd->UserCallback == ImDrawCallback_ResetRenderState )
                        ImGui_ImplRHI_SetupRenderState( a_RHI );
                    else
                        pcmd->UserCallback( cmd_list, pcmd );
                }
                else
                {
                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec2 clip_min( ( pcmd->ClipRect.x - clipOff.x ) * clipScale.x, ( pcmd->ClipRect.y - clipOff.y ) * clipScale.y );
                    ImVec2 clip_max( ( pcmd->ClipRect.z - clipOff.x ) * clipScale.x, ( pcmd->ClipRect.w - clipOff.y ) * clipScale.y );
                    if ( clip_min.x < 0.0f ) { clip_min.x = 0.0f; }
                    if ( clip_min.y < 0.0f ) { clip_min.y = 0.0f; }
                    if ( clip_max.x > ( float )fbWidth ) { clip_max.x = ( float )fbWidth; }
                    if ( clip_max.y > ( float )fbHeight ) { clip_max.y = ( float )fbHeight; }
                    if ( clip_max.x <= clip_min.x || clip_max.y <= clip_min.y )
                        continue;

                    const auto vpState = RHIViewportState{}
                        .AddViewport( RHIViewport{
                            .X = 0.0f, .Y = 0.0f,
                            .Width = ( float )fbWidth,
                            .Height = ( float )fbHeight,
                            .MinDepth = 0.0f, .MaxDepth = 1.0f
                        } )
                        .AddScissor( RHIScissorRect{
                            .Left = ( uint16_t )clip_min.x,
                            .Top = ( uint16_t )clip_min.y,
                            .Right = ( uint16_t )clip_max.x,
                            .Bottom = ( uint16_t )clip_max.y
                        } );

                    const auto drawArgs = RHIDrawArgs{}
                        .SetIndexCount( pcmd->ElemCount )
                        .SetBaseVertex( pcmd->VtxOffset + vtxOffset )
                        .SetBaseIndex( pcmd->IdxOffset + idxOffset );

					IRHITexture* texture = (IRHITexture*)pcmd->GetTexID();

                    graphicsState.BindingSets[ 0 ] = bd->GetOrCreateBindingSet( texture ).get();

					if ( texture && ( 
                        texture->Desc().Format == ERHIFormat::D32_FLOAT || 
                        texture->Desc().Format == ERHIFormat::D24_UNORM_S8_UINT ||
                        texture->Desc().Format == ERHIFormat::D16_UNORM ) )
                    {
						graphicsState.SetPipelineState( bd->DepthTexturePipelineState.get() );
                    }
                    else
                    {
						graphicsState.SetPipelineState( bd->PipelineState.get() );
                    }

                    cmdList->SetGraphicsState( graphicsState );
                    cmdList->SetInlinedConstants( inlinedConstants, 0 );
                    cmdList->SetViewportState( vpState );
                    cmdList->Draw( drawArgs );
                }
            }

			vtxOffset += cmd_list->VtxBuffer.Size;
			idxOffset += cmd_list->IdxBuffer.Size;
        }

		TODO( "Temp solution, proper state tracking needed" );
		bd->CommandList->ResourceBarrier( RHIResourceBarrier{ a_RenderTarget.get(), ERHIResourceStates::RenderTarget, ERHIResourceStates::Present});

		cmdList->PopDebugGroup();
        bd->CommandList->Close();
		IRHICommandList* cmdLists[] = { bd->CommandList.get() };
		a_RHI->ExecuteCommandLists( { cmdLists, 1 }, ERHICommandQueueType::Graphics );
    }

    // Called by Init/NewFrame/Shutdown
    static bool ImGui_ImplRHI_CreateFontsTexture()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplRHI_Data* bd = ImGui_ImplRHI_GetBackendData();

        // Build texture atlas
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

        // Upload texture to graphics system
        // (Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling)
        const auto fontTexDesc = RHITextureDesc{}
            .SetDimension( ERHITextureDimension::Texture2D )
            .SetWidth( ( uint32_t )width )
            .SetHeight( ( uint32_t )height )
            .SetMips( 1 )
            .SetFormat( ERHIFormat::RGBA8_UNORM )
            .SetBindFlags( ERHIBindFlags::ShaderResource )
			.SetHeapType( ERHIHeapType::Default )
            .SetName( "ImGuiFontAtlas" );

        auto subResource = RHITextureSubresourceData{}
            .SetData( pixels )
            .SetRowStride( ( size_t )( GetRHIFormatInfo( fontTexDesc.Format ).Bytes() * width ) )
            .SetDepthStride( 0 );

        bd->FontTexture = bd->DynamicRHI->CreateTexture( fontTexDesc, { &subResource, 1 } );
        if ( bd->FontTexture == nullptr || !bd->FontTexture->Valid() )
        {
            LOG( LogCategory::Editor, Error, "error creating texture" );
            return false;
        }

        // Store our identifier
        io.Fonts->SetTexID( ( ImTextureID )( intptr_t )bd->FontTexture.get() );

        return true;
    }

    void ImGui_ImplRHI_DestroyFontsTexture()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplRHI_Data* bd = ImGui_ImplRHI_GetBackendData();

        if ( bd->FontTexture )
        {
            io.Fonts->SetTexID( 0 );
            bd->FontTexture = nullptr;
        }
    }

    static bool ImGui_ImplRHI_CreateCommandList()
    {
        ImGui_ImplRHI_Data* bd = ImGui_ImplRHI_GetBackendData();
        if ( bd->CommandList == nullptr )
        {
            const auto cmdListDesc = RHICommandListDesc{}
                .SetEnableImmediateExecution( false )
                .SetQueueType( ERHICommandQueueType::Graphics )
                .SetName( "ImGui Command List" );

            bd->CommandList = bd->DynamicRHI->CreateCommandList( cmdListDesc );
            if ( bd->CommandList == nullptr || !bd->CommandList->Valid() )
            {
                LOG( LogCategory::Editor, Error, "Failed to create command list for ImGui backend!" );
                return false;
            }
        }
        return true;
    }

    static bool ImGui_ImplRHI_CreatePipelineState()
    {
        auto* bd = ImGui_ImplRHI_GetBackendData();

        const auto bindingLayoutDesc = RHIBindingLayoutDesc{}
            .SetVisibility( ERHIShaderVisibility::All )
            .SetName( "ImGui Binding Layout" )
			.AddBinding( "inlinedConstants"_H, RHIShaderBinding::InlinedConstants( sizeof( InlinedConstants ) ) )
            .AddBinding( "Texture"_H, RHIShaderBinding::Texture( 0, ERHITextureDimension::Texture2D ) );

        bd->BindingLayout = bd->DynamicRHI->CreateBindingLayout( bindingLayoutDesc );

        if ( bd->BindingLayout == nullptr || !bd->BindingLayout->Valid() )
        {
            LOG( LogCategory::Editor, Error, "Failed to create binding layout for ImGui backend!" );
            return false;
		}

		// Compile vertex shader
        const ShaderCompilerInput vertexCompilerInput
        {
            .Source = s_ImGuiVertShader,
            .ShaderType = ERHIShaderType::Vertex,
            .Format = RHI::GetShaderFormat(),
            .Flags = ERHIShaderCompilerFlags::RowMajor
		};
		auto vertexShaderOutput = RHIShaderCompiler::Compile( vertexCompilerInput );

		if ( vertexShaderOutput.IsError() )
        {
            LOG( LogCategory::Editor, Error, "Failed to compile vertex shader for ImGui backend! Error: {}", vertexShaderOutput.Error() );
            return false;
		}

		// Vertex shader
		RHIShaderModuleRef vertexShader = RHI::CreateShaderModule( RHIShaderModuleDesc{}.SetName( "ImGui Vertex Shader" ).SetType( ERHIShaderType::Vertex ).SetBytecode( vertexShaderOutput.Value().ByteCode ).SetSource( s_ImGuiVertShader ) );
        if ( vertexShader == nullptr || !vertexShader->Valid() )
        {
            LOG( LogCategory::Editor, Error, "Failed to create vertex shader for ImGui backend!" );
            return false;
		}

		// Compile pixel shader
        RHIShaderModuleRef pixelShader;
        {
            const ShaderCompilerInput pixelCompilerInput
            {
                .Source = s_ImGuiPixelShader,
                .ShaderType = ERHIShaderType::Pixel,
                .Format = RHI::GetShaderFormat(),
                .Flags = ERHIShaderCompilerFlags::RowMajor
            };

            auto pixelShaderOutput = RHIShaderCompiler::Compile( pixelCompilerInput );

            if ( pixelShaderOutput.IsError() )
            {
                LOG( LogCategory::Editor, Error, "Failed to compile pixel shader for ImGui backend! Error: %s", pixelShaderOutput.Error().c_str() );
                return false;
            }

            // Pixel shader
            pixelShader = RHI::CreateShaderModule( RHIShaderModuleDesc{}.SetName( "ImGui Pixel Shader" ).SetType( ERHIShaderType::Pixel ).SetBytecode( pixelShaderOutput.Value().ByteCode ).SetSource( s_ImGuiPixelShader ) );
            if ( pixelShader == nullptr || !pixelShader->Valid() )
            {
                LOG( LogCategory::Editor, Error, "Failed to create pixel shader for ImGui backend!" );
                return false;
            }
        }

        // Compile Depth texture pixel shader
        RHIShaderModuleRef depthPixelShader;
        {
            const ShaderCompilerInput pixelCompilerInput
            {
                .Source = s_ImGuiDepthTexturePixelShader,
                .ShaderType = ERHIShaderType::Pixel,
                .Format = RHI::GetShaderFormat(),
                .Flags = ERHIShaderCompilerFlags::RowMajor
            };

            auto pixelShaderOutput = RHIShaderCompiler::Compile( pixelCompilerInput );

            if ( pixelShaderOutput.IsError() )
            {
                LOG( LogCategory::Editor, Error, "Failed to compile depth pixel shader for ImGui backend! Error: %s", pixelShaderOutput.Error().c_str() );
                return false;
            }

            // Pixel shader
            depthPixelShader = RHI::CreateShaderModule( RHIShaderModuleDesc{}.SetName( "ImGui Depth Texture Pixel Shader" ).SetType( ERHIShaderType::Pixel ).SetBytecode( pixelShaderOutput.Value().ByteCode ).SetSource( s_ImGuiDepthTexturePixelShader ) );
            if ( depthPixelShader == nullptr || !depthPixelShader->Valid() )
            {
                LOG( LogCategory::Editor, Error, "Failed to create depth pixel shader for ImGui backend!" );
                return false;
            }
        }

        struct ImGuiVertex
        {
			Vector2 Position;
			Vector2 TEXCOORD;
			Vector<4, uint8_t> Color;
        };

        auto psoDesc = RHIGraphicsPipelineStateDesc{}
            .SetName( "ImGui Pipeline State" )
            .SetVertexShader( vertexShader )
            .SetPixelShader( pixelShader )
            .AddBindingLayout( bd->BindingLayout )
            .SetVertexLayout( RHIVertexLayout::From<ImGuiVertex>() )
            .SetFramebufferInfo( RHIFramebufferInfo{}
                                 .SetColorFormats( { ERHIFormat::RGBA8_UNORM } ) )
            .SetDepthState( RHIDepthState{}
                            .SetDepthTestEnabled( false )
                            .SetDepthWriteEnabled( false ) )
            .SetStencilState( RHIStencilState{}
                              .SetEnabled( false ) )
            .SetTopology( ERHITopology::Triangle )
            .SetRasterizerState( RHIRasterizerState{}
                                 .SetFillMode( ERHIFillMode::Solid )
                                 .SetCullMode( ERHICullMode::None )
                                 .SetClockwise( true )
								 .SetDepthClipEnabled( true ) )
            .SetBlendState( RHIBlendState{}
                            .SetAlphaToCoverageEnabled( false )
                            .SetIndependentBlendEnabled( false )
                            .SetRenderTarget( 0, RHIBlendState::RenderTarget{}
                                              .SetBlendEnabled( true )
                                              .SetLogicOpEnabled( false )
                                              .SetSrcColor( ERHIBlendFactor::SrcAlpha )
                                              .SetDstColor( ERHIBlendFactor::OneMinusSrcAlpha )
                                              .SetSrcAlpha( ERHIBlendFactor::One )
                                              .SetDstAlpha( ERHIBlendFactor::OneMinusSrcAlpha )
                                              .SetBlendOpColor( ERHIBlendOp::Add )
                                              .SetBlendOpAlpha( ERHIBlendOp::Add )
                                              .SetLogicOp( ERHILogicOp::NoOp )
                                              .SetColorWriteMask( ERHIColorMask::RGBA )
                            )
            );

        bd->PipelineState = bd->DynamicRHI->CreateGraphicsPipelineState( psoDesc );

        bd->DepthTexturePipelineState = bd->DynamicRHI->CreateGraphicsPipelineState( psoDesc.SetPixelShader( depthPixelShader ) );

		return bd->PipelineState != nullptr && bd->PipelineState->Valid();
    }

    bool ImGui_ImplRHI_CreateDeviceObjects()
    {
        return ImGui_ImplRHI_CreateFontsTexture() &&
            ImGui_ImplRHI_CreateCommandList() &&
            ImGui_ImplRHI_CreatePipelineState();
    }

    void ImGui_ImplRHI_DestroyDeviceObjects()
    {
        ImGui_ImplRHI_DestroyFontsTexture();
    }

}

//-----------------------------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic pop
#endif