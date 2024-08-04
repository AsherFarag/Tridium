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

	void Renderer::Submit( const SharedPtr<Shader>& a_Shader, const SharedPtr<VertexArray>& a_VertexArray, const Matrix4& a_Transform )
	{
		a_Shader->Bind();
		a_Shader->SetMatrix4( "uPVM", m_SceneData->ViewProjectionMatrix * a_Transform );

		a_VertexArray->Bind();

		RenderCommand::DrawIndexed( a_VertexArray );
	}
}