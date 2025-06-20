#include "tripch.h"
#include "RHI_OpenGLImpl.h"

namespace Tridium::OpenGL {

	void UniformLayout::SetLayout( uint32_t a_LayoutIndex, const IRHIBindingLayout& a_Layout, GLuint a_ShaderProgramID )
	{
		thread_local GLchar uniformNameBuffer[256];
		GLsizei length = 0;
		GLsizei size = 0;


		auto& layout = Layouts[a_LayoutIndex];
		for ( uint32_t i = 0; i < a_Layout.Desc().Bindings.Size(); ++i )
		{
			const RHIShaderBinding& binding = a_Layout.Desc().Bindings[i];

			Uniform& uniform = binding.Type() == ERHIBindingType::InlinedConstants
				? InlinedConstants
				: layout[binding.Slot];

			StringView name = a_Layout.Desc().GetBindingName( binding.NameHash );
			uniform.BindingPoint = OpenGL3::GetUniformLocation( a_ShaderProgramID, name.data() );
			uniform.IsBlock = false;

			if ( uniform.BindingPoint < 0 )
			{
				// The binding is not a uniform, it might be a block or an array
				uniform.BlockIndex = OpenGL3::GetUniformBlockIndex( a_ShaderProgramID, name.data() );
				if ( uniform.BlockIndex < 0 )
					continue; // Not a uniform or block, skip it

				uniform.IsBlock = true;
				uniform.BindingPoint = binding.Slot;
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
