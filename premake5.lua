workspace "Tridium"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug-Editor",
		"Release-Editor",
		"Debug",
		"Release",
		"Shipping"
	}

outputdir = "%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glfw"] = "Tridium/Dependencies/glfw/include"
IncludeDir["glad"] = "Tridium/Dependencies/glad/include"
IncludeDir["ImGui"] = "Tridium/Dependencies/ImGui"
IncludeDir["ImGuizmo"] = "Tridium/Dependencies/ImGuizmo"
IncludeDir["glm"] = "Tridium/Dependencies/glm"
IncludeDir["lua"] = "Tridium/Dependencies/lua/x64"
IncludeDir["sol2"] = "Tridium/Dependencies/sol2"
IncludeDir["entt"] = "Tridium/Dependencies/entt/include"
IncludeDir["stbi"] = "Tridium/Dependencies/stbi/include"
IncludeDir["yaml"] = "Tridium/Dependencies/yaml-cpp"
IncludeDir["assimp"] = "Tridium/Dependencies/assimp"
IncludeDir["refl"] = "Tridium/Dependencies/refl/include"

include "Tridium/Dependencies/glfw"
include "Tridium/Dependencies/glad"
include "Tridium/Dependencies/ImGui"
include "Tridium/Dependencies/assimp"
include "Tridium/Dependencies/yaml-cpp"

project "Tridium"
	location "Tridium"
	kind "StaticLib"
	staticruntime "On"
	language "C++"
	cppdialect "C++20"

	targetdir ("Bin/%{prj.name}/" .. outputdir)
	objdir ("Bin-Int/%{prj.name}/" .. outputdir)

	pchheader "tripch.h"
	pchsource "Tridium/Source/tripch.cpp"

	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
		"%{prj.name}/Dependencies/glm/glm/**.inl",
		"%{prj.name}/Dependencies/glm/glm/**.hpp",
		"%{prj.name}/Dependencies/ImGuizmo/ImGuizmo.h",
		"%{prj.name}/Dependencies/ImGuizmo/ImGuizmo.cpp"
	}

	includedirs
	{
		"%{prj.name}/Dependencies/spdlog/include",
		"%{prj.name}/Source",
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
		"%{IncludeDir.refl}"
	}

	libdirs
	{
		"%{IncludeDir.lua}",
	}

	links
	{
		"glfw",
		"glad",
		"ImGui",
		"lua54",
		"yaml-cpp",
		"assimp",
		"opengl32.lib"
	}

	filter "files:Tridium/Dependencies/ImGuizmo/**.cpp"
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



project "Sandbox"
	location "Tridium"
	kind "ConsoleApp"
	staticruntime "On"
	language "C++"
	cppdialect "C++20"

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
		"%{IncludeDir.lua}/include",
		"%{IncludeDir.sol2}/include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml}/include",
		"%{IncludeDir.stbi}"
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