#include "tripch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Camera.h"
#include "Material.h"

namespace Tridium {
	
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData();

	void Renderer::BeginScene( const Camera& a_Camera, const Matrix4& a_ViewMatrix )
	{
		m_SceneData->ViewProjectionMatrix = a_Camera.GetProjection() * a_ViewMatrix;
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit( const Ref<Shader>& a_Shader, const Ref<VertexArray>& a_VertexArray, const Matrix4& a_Transform )
	{
		a_Shader->Bind();
		a_Shader->SetMatrix4( "uPVM", m_SceneData->ViewProjectionMatrix * a_Transform );

		a_VertexArray->Bind();

		RenderCommand::DrawIndexed( a_VertexArray );
	}

	void Renderer::Submit( const Ref<Material>& a_Material, const Ref<VertexArray>& a_VertexArray, const Matrix4& a_Transform )
	{
		Ref<Shader> shader = nullptr;
		if ( a_Material != nullptr )
			shader = ShaderLibrary::GetShader( a_Material->GetShader() );

		if ( shader == nullptr )
			shader = ShaderLibrary::GetShader( ShaderLibrary::GetDefaultShader() );

		shader->Bind();
		shader->SetMatrix4( "uPVM", m_SceneData->ViewProjectionMatrix * a_Transform );

		if ( a_Material )
		{
			shader->SetFloat3( "uColor", a_Material->Color);
			a_Material->Bind();
		}

		a_VertexArray->Bind();

		RenderCommand::DrawIndexed( a_VertexArray );

		if ( a_Material )
			a_Material->Unbind();
	}

}