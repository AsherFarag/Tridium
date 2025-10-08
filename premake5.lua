include "Dependencies.lua"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

workspace "Tridium"
	startproject "Tridium"
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