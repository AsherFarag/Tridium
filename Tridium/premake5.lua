project "Tridium"
	kind "StaticLib"
	staticruntime "On"
	language "C++"
	cppdialect "C++20"

	targetdir ("Bin/%{prj.name}/" .. outputdir)
	objdir ("Bin-Int/%{prj.name}/" .. outputdir)

	pchheader "tripch.h"
	pchsource "Source/tripch.cpp"

	files
	{
		"Source/**.h",
		"Source/**.cpp",
		"Dependencies/glm/glm/**.inl",
		"Dependencies/glm/glm/**.hpp",
		"Dependencies/ImGuizmo/ImGuizmo.h",
		"Dependencies/ImGuizmo/ImGuizmo.cpp",
		"Dependencies/HdriToCubemap/HdriToCubemap.hpp"
	}

	includedirs
	{
		"Source",
        "%{IncludeDir.spdlog}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.lua}/include",
		"%{IncludeDir.sol2}/include",
		"%{IncludeDir.entt}",
		"%{IncludeDir.stbi}",
		"%{IncludeDir.yaml}/include",
		"%{IncludeDir.assimp}/include",
		"%{IncludeDir.refl}",
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
	}

	links
	{
		"GLFW.lib",
		"glad.lib",
		"ImGui.lib",
		"lua54.lib",
		"yaml-cpp.lib",
		"assimp.lib",
		"opengl32.lib"
	}

	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}

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
			"IS_EDITOR"
		}
		symbols "On"

	filter "configurations:Release-Editor"
		defines
		{ 
			"TE_RELEASE",
			"IS_EDITOR"
		}
		optimize "On"

	filter "configurations:Debug"
		defines "TE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "TE_RELEASE"
		optimize "On"

	filter "configurations:Shipping"
		defines "TE_SHIPPING"
		optimize "On"