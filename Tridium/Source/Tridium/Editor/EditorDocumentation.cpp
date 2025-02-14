#include "tripch.h"
#include "EditorDocumentation.h"

namespace Tridium {

	void RegisterGameObjectDocumentation( EditorDocumentation& a_Storage );
	void RegisterComponentDocumentation( EditorDocumentation& a_Storage );

	void EditorDocumentation::Init()
	{
		RegisterGameObjectDocumentation( *this );
		RegisterComponentDocumentation( *this );
	}

	void RegisterGameObjectDocumentation( EditorDocumentation& a_Storage )
	{
		TypeDocumentation doc;
		doc.Name = "GameObject";
		doc.Description = "A GameObject is the base entity in the ECS system. It is a container for components and can be parented to other GameObjects.";
		doc.Members = { };
		doc.Functions = { };
		a_Storage.RegisterType( doc );
	}

	void RegisterComponentDocumentation( EditorDocumentation& a_Storage )
	{
		// Native Script Components
		{
			// NativeScriptComponent
			{
				TypeDocumentation doc;
				doc.Name = "NativeScriptComponent";
				doc.Description = "A component that allows you to write scripting functionality and attach it to a GameObject.\n"
					"Inheriting from this class allows you to override script functions such as OnBeginPlay, OnUpdate, and OnEndPlay.\n"
					"This component also stores a bitflags to determine a component's state such as whether it is enabled or disabled, visible or invisible, etc.\n"
					"No virtual functions are used, so you must call the base class's function manually if you override a scripting function.\n"
					"Example:\n"
					"void OnBeginPlay()\n"
					"{\n"
					"    Super::OnBeginPlay();\n"
					"    // Your code here\n"
					"}";
				doc.Members = {
					{ "GameObject", "The GameObject this component is attached to.", "GameObject" },
					{ "ComponentFlags", "Bit flags that represent the state of this component.", "uint32_t" }
				};
				doc.Functions = { 
					{ "OnBeginPlay", "Called before the first frame or when this component is created.", "void" },
					{ "OnUpdate", "Called every frame.", "void", { { "a_DeltaTime", "The time since the last frame." } } },
					{ "OnEndPlay", "Called when this component is destroyed.", "void" },
					{ "GetScene", "Returns the scene this GameObject is in.", "const Scene*" },
					{ "HasComponent", "Returns true if the GameObject has a component of type T.", "template<typename T> bool" },
					{ "TryGetComponent", "Returns a pointer to a component of type T if it exists, otherwise returns nullptr.", "template<typename T> T*" },
					{ "GetComponent", "Returns a reference to a component of type T.", "template<typename T> T&" },
					{ "AddComponent", "Adds a component of type T to the GameObject.", "template<typename T, typename... _Args> T&", { { "a_Args", "Arguments to pass to the component's constructor." } } }
				};
				a_Storage.RegisterType( doc );
			}

			// TransformComponent
			{
				TypeDocumentation doc;
				doc.Name = "TransformComponent";
				doc.Description =
					"A component that represents the position, rotation, and scale of a GameObject in 3D space.\n"
					"It is also in charge of parenting and child relationships between GameObjects.";
				doc.Members = { 
					{ "Position", "The position of the GameObject in world space.", "Vector3" },
					{ "Rotation", "The rotation of the GameObject in world space.", "Quaternion" },
					{ "Scale", "The scale of the GameObject in world space.", "Vector3" },
					{ "Parent", "The parent GameObject of this GameObject.", "GameObject" },
					{ "Children", "The children GameObjects of this GameObject.", "Array<GameObject>" }
				};
				doc.Functions = {
					{ "SetWorldPosition", "Sets the world position of the GameObject.", "void", { { "a_Position", "The new world position." } } },
					{ "SetWorldRotation", "Sets the world rotation of the GameObject.", "void", { { "a_Rotation", "The new world rotation." } } },
					{ "SetWorldScale", "Sets the world scale of the GameObject.", "void", { { "a_Scale", "The new world scale." } } },
					{ "GetWorldTransform", "Returns the world transform of the GameObject.", "Matrix4" }
				};
				a_Storage.RegisterType( doc );
			}

			// StaticMeshComponent
			{
				TypeDocumentation doc;
				doc.Name = "StaticMeshComponent";
				doc.Description = "A component that represents a static mesh that can be rendered in the scene.";
				doc.Members = {
					{ "Mesh", "The mesh asset to render.", "AssetHandle<StaticMesh>"  },
					{ "Materials", "The material asset to use when rendering the mesh.", "Array<MaterialHandle>" }
				};
				doc.Functions = { };
				a_Storage.RegisterType( doc );
			}
		}
	}

}
