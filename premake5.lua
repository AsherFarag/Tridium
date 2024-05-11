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
	kind "SharedLib"
	language "C++"

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
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
				"TE_PLATFORM_WINDOWS",
				"TE_BUILD_DLL",
				"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../Bin/Sandbox/".. outputdir)
		}

	filter "configurations:Debug"
		defines "TRI_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "TRI_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "TRI_DIST"
		buildoptions "/MD"
		optimize "On"



project "Sandbox"
	location "Tridium"
	kind "ConsoleApp"
	language "C++"

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
		"Tridium",
		"ImGui"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
				"TE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "TRI_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "TRI_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "TRI_DIST"
		runtime "Release"
		optimize "On"