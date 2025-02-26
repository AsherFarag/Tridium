#include "tripch.h"
#include "D3D12CommandList.h"
#include "D3D12PipelineState.h"
#include "D3D12Texture.h"
#include "D3D12Mesh.h"
#include "D3D12DynamicRHI.h"
#include "D3D12ShaderBindingLayout.h"

namespace Tridium {

	bool D3D12CommandList::SetCommands( const RHICommandBuffer& a_CmdBuffer )
	{
		D3D12RHI* rhi = RHI::GetD3D12RHI();
		CHECK( rhi );

		const auto& cmdAllocator = rhi->GetCommandAllocator();
		// Reset the command allocator
		if ( FAILED( cmdAllocator->Reset() ) )
		{
			return false;
		}

		// Reset the command list
		if ( FAILED( CommandList->Reset( cmdAllocator.Get(), nullptr ) ) )
		{
			return false;
		}

		thread_local RHIShaderBindingLayout* s_CurrentSBL = nullptr;

		struct ScopeGuard
		{
			~ScopeGuard()
			{
				s_CurrentSBL = nullptr;
			}
		} guard;

		// Execute the commands
		for ( const RHICommand& cmd : a_CmdBuffer.Commands )
		{
			switch ( cmd.Type() )
			{
				using enum ERHICommandType;
				case SetPipelineState:
				{
					const auto& data = cmd.Get<SetPipelineState>();
					CommandList->SetPipelineState( data.PSO->As<D3D12PipelineState>()->PSO.Get() );
					break;
				}
				case SetShaderBindingLayout:
				{
					const auto& data = cmd.Get<SetShaderBindingLayout>();
					s_CurrentSBL = data.SBL;
					CommandList->SetGraphicsRootSignature( data.SBL->As<D3D12ShaderBindingLayout>()->m_RootSignature.Get() );
					break;
				}
				case SetRenderTargets:
				{
					const auto& data = cmd.Get<SetRenderTargets>();
					if ( data.RTV.Size() == 0 )
					{
						CommandList->OMSetRenderTargets( 0, nullptr, FALSE, nullptr );
						break;
					}

					D3D12_CPU_DESCRIPTOR_HANDLE rtvs[RHIQuery::MaxColorTargets];
					for ( size_t i = 0; i < data.RTV.Size(); ++i )
					{
						rtvs[i] = data.RTV[i]->As<D3D12Texture>()->DescriptorHandle;
					}

					if ( data.DSV )
					{
						D3D12_CPU_DESCRIPTOR_HANDLE dsv = data.DSV->As<D3D12Texture>()->DescriptorHandle;
						CommandList->OMSetRenderTargets( data.RTV.Size(), rtvs, FALSE, &dsv );
					}
					else
					{
						CommandList->OMSetRenderTargets( data.RTV.Size(), rtvs, FALSE, nullptr );
					}
					break;
				}
				case ClearRenderTargets:
				{
					const auto& data = cmd.Get<ClearRenderTargets>();
					for ( size_t i = 0; i < data.RTV.Size(); ++i )
					{
						CommandList->ClearRenderTargetView( data.RTV[i]->As<D3D12Texture>()->DescriptorHandle, &data.ClearColor[0], 0, nullptr);
					}
					break;
				}
				case SetScissors:
				{
					const auto& data = cmd.Get<SetScissors>();
					TODO( "I just slapped a thread_local on this as I'm not sure if D3D12 needs me to keep the data around" );
					thread_local RECT rects[RHIQuery::MaxColorTargets];
					for ( size_t i = 0; i < data.Rects.Size(); ++i )
					{
						const auto& rect = data.Rects[i];
						rects[i].left = rect.Left;
						rects[i].top = rect.Top;
						rects[i].right = rect.Right;
						rects[i].bottom = rect.Bottom;
					}

					CommandList->RSSetScissorRects( data.Rects.Size(), rects );
					break;
				}
				case SetViewports:
				{
					// Check the offsets of the Tridium Viewport and D3D12_VIEWPORT variables are the same so we can use a cast
					static_assert( sizeof( Viewport ) == sizeof( D3D12_VIEWPORT ) );
					static_assert( offsetof( Viewport, X ) == offsetof( D3D12_VIEWPORT, TopLeftX ) );
					static_assert( offsetof( Viewport, Y ) == offsetof( D3D12_VIEWPORT, TopLeftY ) );
					static_assert( offsetof( Viewport, Width ) == offsetof( D3D12_VIEWPORT, Width ) );
					static_assert( offsetof( Viewport, Height ) == offsetof( D3D12_VIEWPORT, Height ) );
					static_assert( offsetof( Viewport, MinDepth ) == offsetof( D3D12_VIEWPORT, MinDepth ) );
					static_assert( offsetof( Viewport, MaxDepth ) == offsetof( D3D12_VIEWPORT, MaxDepth ) );

					const auto& data = cmd.Get<SetViewports>();
					CommandList->RSSetViewports( data.Viewports.Size(), reinterpret_cast<const D3D12_VIEWPORT*>( data.Viewports.Data() ) );
					break;
				}
				case SetShaderInput:
				{
					const auto& data = cmd.Get<SetShaderInput>();
					if ( !s_CurrentSBL )
					{
						ASSERT_LOG( false, "No Shader Binding Layout set!" );
						break;
					}

					const RHIShaderBindingLayoutDescriptor* desc = s_CurrentSBL->GetDescriptor();
					const RHIShaderBinding& binding = desc->Bindings.At( data.Index );

					switch ( binding.BindingType )
					{
						case ERHIShaderBindingType::Constant:
						{
							if ( binding.IsInlined )
							{
								CommandList->SetGraphicsRoot32BitConstants( data.Index, binding.WordSize, static_cast<const void*>( &data.Payload.InlineData[0] ), 0 );
							}
							else
							{

							}
							break;
						}
						case ERHIShaderBindingType::Texture:
						{
							break;
						}
						default:
						{
							ASSERT_LOG( false, "Unknown Shader Binding Type!" );
							break;
						}
					}
					break;
				}
				case SetIndexBuffer:
				{
					const auto& data = cmd.Get<SetIndexBuffer>();
					break;
				}
				case SetVertexBuffer:
				{
					const auto& data = cmd.Get<SetVertexBuffer>();
					D3D12_VERTEX_BUFFER_VIEW vbv{};
					vbv.BufferLocation = data.VBO->As<D3D12VertexBuffer>()->VBO->GetGPUVirtualAddress();
					vbv.SizeInBytes = static_cast<UINT>( data.VBO->As<D3D12VertexBuffer>()->VBO->GetDesc().Width );
					vbv.StrideInBytes = static_cast<UINT>( data.VBO->GetDescriptor()->Layout.Stride );
					CommandList->IASetVertexBuffers( 0, 1, &vbv );
					break;
				}
				case SetPrimitiveTopology:
				{
					const auto& data = cmd.Get<SetPrimitiveTopology>();
					D3D_PRIMITIVE_TOPOLOGY topology;
					switch ( data.Topology )
					{
					case ERHITopology::Point:
						topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
						break;
					case ERHITopology::Triangle:
						topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
						break;
					case ERHITopology::TriangleStrip:
						topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
						break;
					case ERHITopology::Line:
						topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
						break;
					case ERHITopology::LineStrip:
						topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
						break;
					default:
						topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
						break;
					}

					CommandList->IASetPrimitiveTopology( topology );
					break;
				}
				case Draw:
				{
					const auto& data = cmd.Get<Draw>();
					CommandList->DrawInstanced( data.VertexCount, 1, data.VertexStart, 0 );
					break;
				}
				case DrawIndexed:
				{
					const auto& data = cmd.Get<DrawIndexed>();
					CommandList->DrawIndexedInstanced( data.IndexCount, 1, data.IndexStart, 0, 0 );
					break;
				}
				case ResourceBarrier:
				{
					const auto& data = cmd.Get<ResourceBarrier>();
					D3D12_RESOURCE_BARRIER barrier = {};
					barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
					barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
					barrier.Transition.pResource = data.Resource->NativePtrAs<ID3D12Resource*>();
					barrier.Transition.StateBefore = ToD3D12::GetResourceState( data.Before );
					barrier.Transition.StateAfter = ToD3D12::GetResourceState( data.After );
					barrier.Transition.Subresource = 0;
					CommandList->ResourceBarrier( 1, &barrier );
					break;
				}
				case DispatchCompute:
				{
					const auto& data = cmd.Get<DispatchCompute>();
					break;
				}
				case FenceSignal:
				{
					const auto& data = cmd.Get<FenceSignal>();
					break;
				}
				case FenceWait:
				{
					const auto& data = cmd.Get<FenceWait>();
					break;
				}
				case Execute:
				{
					const auto& data = cmd.Get<Execute>();
					break;
				}
				default:
				{
					return false;
				}
			}
		}

		return true;
	}

	bool D3D12CommandList::Commit( const void* a_Params )
	{
		const auto* desc = ParamsToDescriptor<RHICommandListDescriptor>( a_Params );
		if ( !desc )
		{
			return false;
		}

		D3D12RHI* rhi = RHI::GetD3D12RHI();
		if ( FAILED( rhi->GetDevice()->CreateCommandList1( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS( &CommandList ) ) ) )
		{
			return false;
		}

		return true;
	}

	bool D3D12CommandList::Release()
	{
		return false;
	}

	bool D3D12CommandList::IsValid() const
	{
		return false;
	}

	const void* D3D12CommandList::NativePtr() const
	{
		return nullptr;
	}

}