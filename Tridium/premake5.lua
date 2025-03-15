project "Tridium"
	kind "StaticLib"
	staticruntime "On"
	language "C++"
	cppdialect "C++20"

	targetdir ("Bin/%{prj.name}/" .. outputdir)
	objdir ("Bin-Int/%{prj.name}/" .. outputdir)

	pchheader "tripch.h"
	pchsource "Source/tripch.cpp"

	dependson 
	{ 
		"assimp",
		"JoltPhysics",
		"GLFW",
		"glad",
		"ImGui",
		"yaml-cpp",
	}

	files
	{
		"Source/**.h",
		"Source/**.cpp",
		"Source/**.ixx",
		"Dependencies/glm/glm/**.inl",
		"Dependencies/glm/glm/**.hpp",
		"Dependencies/ImGuizmo/ImGuizmo.h",
		"Dependencies/ImGuizmo/ImGuizmo.cpp",
		"%{IncludeDir.ImTextEdit}/ImTextEdit/TextEditor.h",
		"%{IncludeDir.ImTextEdit}/ImTextEdit/TextEditor.cpp",
		"Dependencies/HdriToCubemap/HdriToCubemap.hpp",
		"%{IncludeDir.sol2}/include/sol/**.hpp",
		"%{IncludeDir.sol2}/include/sol/**.h",
	}

	includedirs
	{
		"Source",
		"Shaders",
        "%{IncludeDir.spdlog}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.ImTextEdit}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.lua}/include",
		"%{IncludeDir.sol2}/include",
		"%{IncludeDir.entt}",
		"%{IncludeDir.stbi}",
		"%{IncludeDir.yaml}/include",
		"%{IncludeDir.assimp}/include",
		"%{IncludeDir.refl}",
		"%{IncludeDir.JoltPhysics}",
		"Dependencies/HdriToCubemap"
	}

	libdirs
	{
		"%{IncludeDir.lua}",
		"Dependencies/glfw/bin/" .. outputdir .. "/GLFW",
		"Dependencies/glad/bin/" .. outputdir .. "/glad",
		"Dependencies/ImGui/bin/" .. outputdir .. "/ImGui",
		"Dependencies/yaml-cpp/bin/" .. outputdir .. "/yaml-cpp",
		"Dependencies/assimp/bin/" .. outputdir .. "/assimp",
		"Dependencies/JoltPhysics/bin/" .. outputdir .. "/JoltPhysics",
	}

	links
	{
		"GLFW.lib",
		"glad.lib",
		"ImGui.lib",
		"lua54.lib",
		"yaml-cpp.lib",
		"assimp.lib",
		"JoltPhysics.lib",
		-- Graphics API's --
		-- OpenGL
		"opengl32.lib",
		-- DirectX 12
		"d3d12.lib",
		"dxgi.lib",
		"dxguid.lib"
	}

	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}

	filter { "files:**.ixx" }
		compileas "Module" -- Treat .ixx files as modules in supported compilers
		flags {"NoPCH"}

	filter "files:Dependencies/stbi/include/stb_image.h"
		optimize "On"

	filter "files:Dependencies/ImGuizmo/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"TE_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}
	
	filter "configurations:Debug-Editor"
		defines
		{ 
			"TE_DEBUG",
			"IS_EDITOR",
			"JPH_DEBUG_RENDERER"
		}
		symbols "On"

	filter "configurations:Release-Editor"
		defines
		{ 
			"TE_RELEASE",
			"IS_EDITOR",
			"JPH_DEBUG_RENDERER"
		}
		optimize "On"

	filter "configurations:Debug"
		defines 
		{
			"TE_DEBUG",
			"JPH_DEBUG_RENDERER"
		}
		symbols "On"

	filter "configurations:Release"
		defines 
		{
			"JPH_DEBUG_RENDERER",
			"TE_RELEASE"
		}
		optimize "On"

	filter "configurations:Shipping"
		defines "TE_SHIPPING"
		optimize "On"