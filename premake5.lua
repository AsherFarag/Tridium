workspace "Tridium"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Tridium/Dependencies/GLFW/include"
IncludeDir["glad"] = "Tridium/Dependencies/glad/include"
IncludeDir["ImGui"] = "Tridium/Dependencies/ImGui"
IncludeDir["glm"] = "Tridium/Dependencies/glm"

include "Tridium/Dependencies/GLFW"
include "Tridium/Dependencies/glad"
include "Tridium/Dependencies/ImGui"

project "Tridium"
	location "Tridium"
	kind "StaticLib"
	staticruntime "On"
	language "C++"
	cppdialect "C++17"

	targetdir ("Bin/%{prj.name}/" .. outputdir)
	objdir ("Bin-Int/%{prj.name}/" .. outputdir)

	pchheader "tripch.h"
	pchsource "Tridium/Source/tripch.cpp"

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
		"%{prj.name}/Dependencies/glm/glm/**.inl",
		"%{prj.name}/Dependencies/glm/glm/**.hpp"
	}

	includedirs
	{
		"%{prj.name}/Dependencies/spdlog/include",
		"%{prj.name}/Source",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"TE_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "TRI_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "TRI_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "TRI_DIST"
		optimize "On"



project "Sandbox"
	location "Tridium"
	kind "ConsoleApp"
	staticruntime "On"
	language "C++"
	cppdialect "C++17"

	targetdir ("Bin/%{prj.name}/" .. outputdir)
	objdir ("Bin-Int/%{prj.name}/" .. outputdir)

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs
	{
		"Tridium/Dependencies/spdlog/include",
		"Tridium/Source",
		"Tridium/Dependencies",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Tridium"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"TE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "TRI_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "TRI_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "TRI_DIST"
		optimize "On"