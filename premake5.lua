include "Dependencies.lua"
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

include "Tridium"
include "Sandbox"
include "Tridium/Dependencies/glfw"
include "Tridium/Dependencies/glad"
include "Tridium/Dependencies/ImGui"
include "Tridium/Dependencies/assimp"
include "Tridium/Dependencies/yaml-cpp"
include "Tridium/Dependencies/JoltPhysics"
include "Tridium/Dependencies/SPIRV-Cross"