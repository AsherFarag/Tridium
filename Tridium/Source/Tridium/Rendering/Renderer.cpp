#include "tripch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include <Tridium/ECS/Components/Types.h>

namespace Tridium {
	
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData();

	void Renderer::BeginScene( const Camera& a_Camera, const TransformComponent& a_CameraTransform )
	{
		Matrix4 ViewMatrix = glm::lookAt( a_CameraTransform.Position, a_CameraTransform.Position + a_CameraTransform.GetForward(), Vector3(0, 1, 0));
		//Matrix4 ViewMatrix = glm::inverse( a_CameraTransform.GetTransform() );
		m_SceneData->ViewProjectionMatrix = a_Camera.GetProjection() * ViewMatrix;
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit( const Ref<Shader>& a_Shader, const Ref<VertexArray>& a_VertexArray, const Matrix4& a_Transform )
	{
		a_Shader->Bind();
		a_Shader->SetMatrix4( "uPVM", m_SceneData->ViewProjectionMatrix );

		a_VertexArray->Bind();

		RenderCommand::DrawIndexed( a_VertexArray );
	}

}