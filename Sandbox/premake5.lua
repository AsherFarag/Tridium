require ("utils")
include "Dependencies"

tridium_app_folder = "."
tridium_core_source_folder = "../Tridium/Source"
tridium_core_shaders_folder = "../Tridium/Shaders"
tridium_core_dependencies_folder = "../Tridium/Dependencies"

local vulkan_sdk = os.getenv("VULKAN_SDK")
if not vulkan_sdk then
    error("VULKAN_SDK environment variable not set. Please install the Vulkan SDK.")
end

tridium_app_dependencies = {
   	"spdlog",
	"glfw",
	"glad",
	"ImGui",
	"ImGuizmo",
	"ImTextEdit",
	"glm",
	"lua",
	"sol2",
	"entt",
	"stbi",
	"yaml-cpp",
	"assimp",
	"refl",
	"JoltPhysics",
	"SPIRV-Cross",
	"HdriToCubemap",
}

project "TridiumApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	buildoptions { "/bigobj", "/Zc:preprocessor" }
	includedirs { tridium_core_source_folder, tridium_core_shaders_folder, tridium_app_folder }
	add_dependencies(tridium_core_dependencies_folder, tridium_app_dependencies, true)

	-- TEMP
	dependson { 
		"assimp",
		"JoltPhysics",
		"GLFW",
		"glad",
		"ImGui",
		"yaml-cpp",
		"SPIRV-Cross"
	}

	-- TEMP
	includedirs {
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
		"../Tridium/Dependencies/SPIRV-Cross",
		"../Tridium/Dependencies/HdriToCubemap",

		-- Vulkan SDK
		vulkan_sdk .. "/Include",
	}

	libdirs {
		"../Tridium/Dependencies/lua/x64",
		vulkan_sdk .. "/Lib",
	}



	links {
		"GLFW.lib",
		"glad.lib",
		"ImGui.lib",
		"lua54.lib",
		"yaml-cpp.lib",
		"assimp.lib",
		"JoltPhysics.lib",
		"SPIRV-Cross.lib",
		-- Graphics API's --
		-- OpenGL
		"opengl32.lib",
		-- DirectX 12
		"d3d12.lib",
		"dxgi.lib",
		"dxguid.lib",
		-- Vulkan
		"vulkan-1"
	}

	-- TEMP
	print("Adding library directories for configuration: " .. outputdir)
    for _, libpath in ipairs(os.matchdirs("../Bin/*")) do
		print( libpath )
        libdirs { libpath .. "/" .. outputdir }
    end

	files
	{
		"../Tridium/Dependencies/glm/glm/**.inl",
		"../Tridium/Dependencies/glm/glm/**.hpp",
		"../Tridium/Dependencies/ImGuizmo/ImGuizmo.h",
		"../Tridium/Dependencies/ImGuizmo/ImGuizmo.cpp",
		"../Tridium/Dependencies/HdriToCubemap/HdriToCubemap.hpp",
		"%{IncludeDir.ImTextEdit}/ImTextEdit/TextEditor.h",
		"%{IncludeDir.ImTextEdit}/ImTextEdit/TextEditor.cpp",
		"%{IncludeDir.sol2}/include/sol/**.hpp",
		"%{IncludeDir.sol2}/include/sol/**.h",
	}

	-- Enable Multiprocessor Compilation for Visual Studio
	filter "action:vs*"
        flags { "MultiProcessorCompile" }

	files {
		path.join(tridium_app_folder, "**.h"),
		path.join(tridium_app_folder, "**.hpp"),
		path.join(tridium_app_folder, "**.inl"),
		path.join(tridium_app_folder, "**.c"),
		path.join(tridium_app_folder, "**.cpp"),
		path.join(tridium_app_folder, "**.natvis"),
	}

	files {
		path.join(tridium_core_source_folder, "**.c"),
		path.join(tridium_core_source_folder, "**.cpp"),
		path.join(tridium_core_source_folder, "**.h"),
		path.join(tridium_core_source_folder, "**.hpp"),
		path.join(tridium_core_source_folder, "**.inl"),
		path.join(tridium_core_source_folder, "**.natvis"),
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