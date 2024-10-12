#include <Tridium.h>

struct PersonInfo
{
	int Age = 0;
	std::string Name = "John Doe";
	float Height = 0.0f;
	bool IsMale = true;
};

BEGIN_REFLECT( PersonInfo )
	PROPERTY( Age, FLAGS( EditAnywhere ) )
	PROPERTY( Name, FLAGS( EditAnywhere ) )
	PROPERTY( Height, FLAGS( EditAnywhere ) )
	PROPERTY( IsMale, FLAGS( EditAnywhere ) )
END_REFLECT( PersonInfo )

class Person : public Tridium::Component
{
	REFLECT(Person);
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