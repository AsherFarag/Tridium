#include <Tridium.h>
using namespace Tridium;
#include <Tridium/Rendering/Material.h>
#include <Tridium/IO/MaterialSerializer.h>
#include <Tridium/IO/SceneSerializer.h>

class ExampleLayer : public Tridium::Layer
{

public:

	ExampleLayer() = default;

	virtual void OnImGuiDraw() override
	{
		//ImGui::ShowDemoWindow();
	}
};

class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{
		SetScene();

		PushLayer( new ExampleLayer() );
	}

	~Sandbox()
	{

	}

	void SetScene()
	{
		// TEMP
		std::string vertexSrc =
			R"(
						#version 410

						layout(location = 0) in vec3 aPosition;
						layout(location = 1) in vec3 aNormal;
						layout(location = 2) in vec2 aUV;
						layout(location = 3) in vec3 aTangent;
						
						out vec4 vPosition;
						out vec3 vNormal;
						out vec2 vUV;
						out vec3 vTangent;
						out vec3 vBiTangent;
						out float vDepth;			
						
						uniform mat4 uPVM;
						
						void main()
						{	
							vPosition    = vec4(aPosition, 1);
							vNormal      = aNormal;
							vUV			 = aUV;
							vTangent     = aTangent;
							vBiTangent   = cross(vNormal, vTangent);
							gl_Position  = uPVM * vec4(aPosition, 1);
							vDepth	     = gl_Position.z;
						}
					)";

		std::string fragSrc =
			R"(
						#version 410 core
						
						out vec4 oFragColor;

						in vec4 vPosition;
						in vec3 vNormal;
						in vec2 vUV;
						in vec3 vTangent;				
						in vec3 vBiTangent;
						in float vDepth;	
	
						void main()
						{
							oFragColor = vec4(vNormal.x, vNormal.y, vNormal.z, 1);
						}
					)";

		Shader::Create( "Default", vertexSrc, fragSrc );

		// TEMP
		vertexSrc =
			R"(
						#version 410

						layout(location = 0) in vec3 aPosition;
						layout(location = 1) in vec3 aNormal;
						layout(location = 2) in vec2 aUV;
						
						out vec4 vPosition;
						out vec3 vNormal;
						out vec2 vUV;			
						
						uniform mat4 uPVM;
						
						void main()
						{	
							gl_Position = uPVM * vec4(aPosition, 1);
							vPosition =  vec4(aPosition, 1);
							vNormal = aNormal;
							vUV = aUV;
						}
					)";

		fragSrc =
			R"(
						#version 410 core

						out vec4 oFragColour;

						in vec4 vPosition;
						in vec3 vNormal;
						in vec2 vUV;					
						
						uniform sampler2D uTexture;

						void main()
						{
							oFragColour = vec4(texture(uTexture, vUV).rgb, 1);
						}
					)";

		Shader::Create( "Texture", vertexSrc, fragSrc );

		//Ref<Material> mat = MakeRef<Material>();
		//mat->_SetHandle( MaterialHandle::Create() );
		//mat->SetShader( ShaderLibrary::GetShaderHandle( "Default" ) );
		//MaterialSerializer ser( mat );
		//ser.SerializeText( "Content/TestMat.tasset" );

	#ifndef IS_EDITOR
			SceneSerializer serializer( Application::GetScene() );
			serializer.DeserializeText( ( Application::GetAssetDirectory() / "Scene.tridium" ).string() );
	#endif // !IS_EDITOR
	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}