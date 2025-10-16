#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	const Uniform* UniformLayout::GetUniform( uint32_t a_BindingSetIndex, ERHIBindingType a_BindingType, uint32_t a_BindingSlot ) const
	{
		if ( a_BindingType == ERHIBindingType::InlinedConstants )
		{
			return &InlinedConstants;
		}

		auto it = Layouts[a_BindingSetIndex].find( a_BindingSlot + (uint32_t)a_BindingType * 1000u );
		if ( it != Layouts[a_BindingSetIndex].end() )
		{
			return &it->second;
		}

		return nullptr;
	}

	void UniformLayout::SetLayout( uint32_t a_LayoutIndex, const IRHIBindingLayout& a_Layout, GLuint a_ShaderProgramID )
	{
		auto& layout = Layouts[a_LayoutIndex];

		for ( uint32_t i = 0; i < a_Layout.Desc().Bindings.Size(); ++i )
		{
			const RHIShaderBinding& binding = a_Layout.Desc().Bindings[i];
			Uniform& uniform = binding.Type() == ERHIBindingType::InlinedConstants
				? InlinedConstants
				: layout[binding.Slot + binding.PackedType * 1000]; // Simple hash to avoid collisions between types

			if ( binding.Type() == ERHIBindingType::StructuredBuffer )
			{
				uniform.BindingPoint = binding.Slot;
				uniform.IsBlock = false;
				continue;
			}

			StringView name = a_Layout.Desc().GetBindingName( binding.NameHash );
			uniform.BindingPoint = OpenGL3::GetUniformLocation( a_ShaderProgramID, name.data() );
			uniform.IsBlock = false;

			if ( uniform.BindingPoint < 0 )
			{
				// The binding is not a uniform, it might be a block or an array
				uniform.BlockIndex = OpenGL3::GetUniformBlockIndex( a_ShaderProgramID, name.data() );

				if ( uniform.BlockIndex < 0 )
				{
					// Not a uniform or block, skip it
					continue;
				}

				uniform.IsBlock = true;
				uniform.BindingPoint = binding.Slot;

				if ( binding.Type() == ERHIBindingType::InlinedConstants )
				{
					TODO( "Hacky solution as for constant buffers bound to slot 0 that clash with the inlined constants slot 0"
						  "Just setting to some arbitrary number for now" );
					uniform.BindingPoint = 16;
				}

				OpenGL3::UniformBlockBinding( a_ShaderProgramID, uniform.BlockIndex, uniform.BindingPoint ); // Map UBO block index → binding point
			}

			LOG( LogCategory::Debug, Debug, "Uniform '{}' at {} (isBlock: {})", name, uniform.BindingPoint, uniform.IsBlock );
		}

	}

	RHIBindingLayout_OpenGLImpl::RHIBindingLayout_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
		: IRHIBindingLayout( a_Device, a_Desc )
	{
	}

	RHIBindingSet_OpenGLImpl::RHIBindingSet_OpenGLImpl( IDynamicRHI* a_Device, const DescriptorType& a_Desc )
		: IRHIBindingSet( a_Device, a_Desc )
	{

	}

}
