include "Dependencies.lua"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

workspace "Tridium"
	startproject "TridiumApp"
	platforms { "Windows" }
	targetdir ("Bin/%{prj.name}/" .. outputdir)
	objdir ("Bin-Int/%{prj.name}/" .. outputdir)

	configurations {
		"Debug-Editor",
		"Release-Editor",
		"Debug",
		"Release",
		"Shipping"
	}

	filter "platforms:windows"
		architecture "x64"

include "Tridium"
include "Sandbox"
include "Tridium/Dependencies/glfw"
include "Tridium/Dependencies/glad"
include "Tridium/Dependencies/ImGui"
include "Tridium/Dependencies/assimp"
include "Tridium/Dependencies/yaml-cpp"
include "Tridium/Dependencies/JoltPhysics"
include "Tridium/Dependencies/SPIRV-Cross"