project "Sandbox"
	kind "ConsoleApp"
	staticruntime "On"
	language "C++"
	cppdialect "C++20"
	buildoptions { "/bigobj" }

	dependson { "Tridium" }

	targetdir ("Bin/%{prj.name}/" .. outputdir)
	objdir ("Bin-Int/%{prj.name}/" .. outputdir)

	files
	{
		"Source/**.h",
		"Source/**.cpp",
		"Source/**.ixx",
	}

	includedirs
	{
		"Source",
        "%{wks.location}/Tridium/Source",
        "%{IncludeDir.spdlog}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.sol2}/include",
		"%{IncludeDir.lua}/include",
        "%{IncludeDir.yaml}/include",
	}

	libdirs
	{
		"%{wks.location}/Tridium/Bin/Tridium/" .. outputdir
	}

	links
	{
		"Tridium.lib"
	}

	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}

	filter { "files:**.ixx" }
		compileas "Module" -- Treat .ixx files as modules in supported compilers
		flags {"NoPCH"}

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