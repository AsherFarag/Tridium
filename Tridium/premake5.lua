require ("Scripts/utils")

tridium_source = "%{wks.location}/Tridium/Source"

-- The local project directory
local projectDir = "%{prj.location}"

-- The local dependencies directory
local depsDir = path.join(projectDir, "Dependencies")

-- The local originals directory
-- Stores the original downloaded/extracted files for dependencies with modifications
local origDir = path.join(projectDir, "Originals")

local vulkansdk = os.getenv("VULKAN_SDK")
if not vulkansdk then
    error("VULKAN_SDK environment variable not set. Please install the Vulkan SDK.")
end

-- Dependency configurations
-- Each dependency can have different configurations for different platforms
--[[ 
	- includeDir: The directory to include for headers
	- sourceDir: The directory to the source files (if set, these files will be built as part of the project)
	- configs: What configurations the dependency is used in (e.g. Debug, Release, Editor, Shipping) if not specified, used in all
	- Platform Configurations (e.g. windows, linux, macos):
		- libDir: The directory to link for libraries
		- libName: The name of the library to link
		- debugLibDir: The directory to link for debug libraries (if different)
		- debugLibName: The name of the debug library to link (if different)
]]
-- TODO: Add a way so dependencies can add their own defines.
local dependencies = {
	-- Assimp: Used for raw model importing and processing (OBJ, FBX, glTF, etc.)
	assimp = {
		includeDir = path.join(depsDir, "assimp", "include"),
		windows = {
			libName  = "assimp-vc143-mt.lib",
			libDir = path.join(depsDir, "assimp", "bin", "windows", "Release"),
			debugLibName = "assimp-vc143-mtd.lib",
			debugLibDir = path.join(depsDir, "assimp", "bin", "windows", "Debug")
		}
	},
	-- EnTT: A fast and reliable entity-component system (ECS)
	entt = {
		includeDir = path.join(depsDir, "entt", "include")
	},
	-- Glad: Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator Based on the Official Specs
	glad = {
		includeDir = path.join(depsDir, "glad", "include"),
		windows = {
			libName = "glad.lib",
			libDir = path.join(depsDir, "glad", "bin", "windows", "Release"),
			debugLibName = "glad.lib",
			debugLibDir = path.join(depsDir, "glad", "bin", "windows", "Debug")
		}
	},
	-- GLFW: A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input
	glfw = {
		includeDir = path.join(depsDir, "glfw", "include"),
		windows = {
			libName = "glfw3.lib",
			libDir = path.join(depsDir, "glfw", "bin", "windows", "Release"),
			debugLibName = "glfw3.lib",
			debugLibDir = path.join(depsDir, "glfw", "bin", "windows", "Debug")
		}
	},
	-- GLM: A header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications
	glm = {
		includeDir = path.join(depsDir, "glm"),
	},
	-- ImGui: Immediate Mode Graphical User interface
	ImGui = {
		includeDir = path.join(depsDir, "ImGui"),
		windows = {
			libName = "ImGui.lib",
			libDir = path.join(depsDir, "ImGui", "bin", "windows", "Release"),
			debugLibName = "ImGui.lib",
			debugLibDir = path.join(depsDir, "ImGui", "bin", "windows", "Debug")
		}
	},
	-- ImGuizmo: Immediate Mode 3D Gizmo for Dear ImGui - Used for object manipulation in the editor
	ImGuizmo = {
		includeDir = path.join(depsDir, "ImGuizmo"),
		sourceDir = path.join(depsDir, "ImGuizmo")
	},
	-- ImTextEdit: Text Editor for Dear ImGui - Used for script editing in the editor
	ImTextEdit = {
		includeDir = path.join(depsDir, "ImTextEdit"),
		sourceDir = path.join(depsDir, "ImTextEdit") 
	},
	-- Jolt Physics: A robust C++ physics engine for games
	JoltPhysics = {
		includeDir = path.join(depsDir, "JoltPhysics"),
		windows = {
			libName = "JoltPhysics.lib",
			libDir = path.join(depsDir, "JoltPhysics", "bin", "windows", "Release"),
			debugLibName = "JoltPhysics.lib",
			debugLibDir = path.join(depsDir, "JoltPhysics", "bin", "windows", "Debug")
		}
	},
	-- Lua: A powerful, efficient, lightweight, embeddable scripting language
	lua = {
		includeDir = path.join(depsDir, "lua", "x64", "include"),
		windows = {
			libName = "lua54",
			libDir = path.join(depsDir, "lua", "x64"),
		}
	},
	-- refl: A C++17 metaprogramming library for reflection
	refl = {
		includeDir = path.join(depsDir, "refl", "Include")
	},
	-- sol2: A C++ wrapper for Lua
	sol2 = {
		includeDir = path.join(depsDir, "sol2", "include")
	},
	-- spdlog: A fast C++ logging library
	spdlog = {
		includeDir = path.join(depsDir, "spdlog", "include"),
		sourceDir = path.join(depsDir, "spdlog", "src")
	},
	-- SPIRV-Cross: A library for parsing and converting SPIR-V to GLSL - Used to convert HLSL to GLSL
	SPIRVCross = {
		includeDir = path.join(depsDir, "SPIRV-Cross", "include"),
		sourceDir = path.join(depsDir, "SPIRV-Cross", "include", "SPIRV-Cross"),
	},
	-- stb_image: A header only image loading library
	stbi = {
		includeDir = path.join(depsDir, "stbi", "include")
	},
	-- Vulkan SDK: The official Vulkan SDK provided by LunarG
	VulkanSDK = {
		includeDir = path.join(vulkansdk, "Include"),
		windows = {
			libDir = path.join(vulkansdk, "Lib"),
			libName = "vulkan-1.lib"
		}
	},
	-- yaml-cpp: A YAML parser and emitter for de/serializing data
	yamlcpp = {
		includeDir = path.join(depsDir, "yaml-cpp", "include"),
		sourceDir = path.join(depsDir, "yaml-cpp", "src"),
		windows = {
			libName = "yaml-cpp.lib",
			libDir = path.join(depsDir, "yaml-cpp", "bin", "windows", "Release"),
			debugLibName = "yaml-cppd.lib",
			debugLibDir = path.join(depsDir, "yaml-cpp", "bin", "windows", "Debug")
		}
	},
	-- zlib: A compression library used by Assimp
	zlib = {
		includeDir = path.join(depsDir, "zlib", "include"),
		windows = {
			libName = "zlibstatic.lib",
			libDir = path.join(depsDir, "zlib", "bin", "windows", "Release"),
			debugLibName = "zlibstaticd.lib",
			debugLibDir = path.join(depsDir, "zlib", "bin", "windows", "Debug")
		}
	}
}

project "Tridium"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	buildoptions { "/bigobj", "/Zc:preprocessor" }

	includedirs {
		tridium_source,
		-- We add 'Tridium/Shaders' so files being compiled by the Shader Compiler and C++ Compiler can have the same include path.
		path.join(tridium_source, "Tridium", "Shaders"),
	}

	files {
		path.join(tridium_source, "**.c"),
		path.join(tridium_source, "**.cpp"),
		path.join(tridium_source, "**.h"),
		path.join(tridium_source, "**.hpp"),
		path.join(tridium_source, "**.inl"),
		path.join(tridium_source, "**.natvis"),
	}

	defines {
		"SPDLOG_COMPILED_LIB",	
		"YAML_CPP_STATIC_DEFINE",
	}

	-- Enable multi-processor compilation for Visual Studio
	filter "action:vs*"
        flags { "MultiProcessorCompile" }

	filter "system:windows"
		systemversion "latest"
		defines {
			"TE_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
			"NOMINMAX",
		}
		links {
			-- OpenGL
			"opengl32.lib",
			-- DirectX 12
			"d3d12.lib",
			"dxgi.lib",
			"dxguid.lib",
		}
	
	filter "configurations:Debug-Editor"
		registerDependencies(dependencies, {"Debug", "Editor"})
		defines
		{ 
			"TE_DEBUG",
			"IS_EDITOR",
			"JPH_DEBUG_RENDERER"
		}
		symbols "On"

	filter "configurations:Release-Editor"
		registerDependencies(dependencies, {"Release", "Editor"})
		defines
		{ 
			"TE_RELEASE",
			"IS_EDITOR",
			"JPH_DEBUG_RENDERER"
		}
		optimize "On"

	filter "configurations:Debug"
		registerDependencies(dependencies, {"Debug"})
		defines 
		{
			"TE_DEBUG",
			"JPH_DEBUG_RENDERER"
		}
		symbols "On"

	filter "configurations:Release"
		registerDependencies(dependencies, {"Release"})
		defines 
		{
			"JPH_DEBUG_RENDERER",
			"TE_RELEASE"
		}
		optimize "On"

	filter "configurations:Shipping"
		registerDependencies(dependencies, {"Shipping"})
		defines "TE_SHIPPING"
		optimize "On"

	filter {}