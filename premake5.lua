workspace "Tridium"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["spdlog"] = "Tridium/Dependencies/spdlog/include"
IncludeDir["GLFW"] = "Tridium/Dependencies/GLFW/include"

include "Tridium/Dependencies/GLFW"

project "Tridium"
	location "Tridium"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")

	pchheader "tripch.h"
	pchsource "Tridium/source/tripch.cpp"

	files
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp"
	}

	includedirs
	{
		"%{IncludeDir.spdlog}",
		"%{prj.name}/source",
		"%{IncludeDir.GLFW}"
	}

	libdirs 
	{
		"%{prj.name}/Dependencies/GLFW/bin/Debug-windows-x86_64/GLFW"
	}

	links
	{
		"GLFW.lib",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
				"TRI_PLATFORM_WINDOWS",
				"TRI_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
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

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp"
	}

	includedirs
	{
		"Tridium/Dependencies/spdlog/include",
		"Tridium/source"
	}

	links
	{
		"Tridium"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
				"TRI_PLATFORM_WINDOWS"
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