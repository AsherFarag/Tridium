require ("utils")

tridium_core_source_folder = "Tridium/Source"
tridium_core_dependencies_folder = "Tridium/Dependencies"

tridium_core_dependencies = {
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

project "TridiumCore"
	kind "SharedItems"
	language "C++"
	includedirs {
		tridium_core_source_folder,
		tridium_core_dependencies_folder,
	}
	files {
		path.join(tridium_core_source_folder, "**.c"),
		path.join(tridium_core_source_folder, "**.cpp"),
		path.join(tridium_core_source_folder, "**.h"),
		path.join(tridium_core_source_folder, "**.hpp"),
		path.join(tridium_core_source_folder, "**.inl"),
		path.join(tridium_core_source_folder, "**.natvis"),
		path.join(tridium_core_dependencies_folder, "**.c"),
		path.join(tridium_core_dependencies_folder, "**.cpp"),
		path.join(tridium_core_dependencies_folder, "**.h"),
		path.join(tridium_core_dependencies_folder, "**.hpp"),
		path.join(tridium_core_dependencies_folder, "**.inl"),
		path.join(tridium_core_dependencies_folder, "**.natvis"),
	}