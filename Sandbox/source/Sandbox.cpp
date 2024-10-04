#include <Tridium.h>

struct PersonInfo
{
	REFLECT;

	~PersonInfo()
	{
		LOG_INFO( "PersonInfo Destructor" );
	}

	int Age = 0;
	std::string Name = "John Doe";
	float Height = 0.0f;
};

BEGIN_REFLECT( PersonInfo )
	PROPERTY( Age, FLAGS( EditAnywhere ) )
	PROPERTY( Name, FLAGS( EditAnywhere ) )
	PROPERTY( Height, FLAGS( EditAnywhere ) )
END_REFLECT( PersonInfo )

class Person : public Tridium::Component
{
	REFLECT;
public:
	PersonInfo Info;
	std::map<std::string, PersonInfo> Children;
	std::vector<int> Numbers = { 1, 2, 3 };
};

BEGIN_REFLECT_COMPONENT( Person )
	PROPERTY( Info, FLAGS( EditAnywhere ) )
	PROPERTY( Children, FLAGS( EditAnywhere ) )
	PROPERTY( Numbers, FLAGS( EditAnywhere ) )
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