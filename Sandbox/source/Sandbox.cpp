#include <Tridium.h>

class Person : public Tridium::Component
{
	REFLECT;
public:
	int Age = 0;
	std::string Name = "John Doe";
	float Height = 0.0f;
};

BEGIN_REFLECT_COMPONENT( Person )
	PROPERTY( Age, FLAGS( EditAnywhere ) )
	PROPERTY( Name, FLAGS( EditAnywhere ) )
	PROPERTY( Height, FLAGS( EditAnywhere ) )
END_REFLECT( Person )

class ExampleLayer : public Tridium::Layer
{

public:

	ExampleLayer() = default;

	virtual void OnImGuiDraw() override
	{
	}
};


class Sandbox : public Tridium::Application
{
public:
	Sandbox()
	{
		PushLayer( new ExampleLayer() );
	}

	~Sandbox()
	{

	}
};

Tridium::Application* Tridium::CreateApplication()
{
	return new Sandbox();
}