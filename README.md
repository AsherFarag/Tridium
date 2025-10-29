# Tridium <img width="30" height="30" alt="EngineIcon" src="https://github.com/user-attachments/assets/7d278b18-99f9-4553-bbad-07164aab1872" /></br>

**Tridium** is a modern 3D game engine built in C++20 with a focus on flexibility, performance, and simplicity. It features an integrated Editor, Physics, ECS (Entity Component System), runtime C++ reflection, serialization, physically-based rendering (PBR), and a dynamically-bound Rendering Hardware Interface (RHI) supporting **OpenGL** and **DirectX 12**. It is still in development with a lot features planned.

> ⚠️ **Important:** This project is currently under active development. Expect bugs and incomplete features.

---
<img width="1920" height="1007" alt="TridiumScreenShotBright" src="https://github.com/user-attachments/assets/4570a0a9-f8a0-47fa-9ac9-ba8fa756900e" />
<br></br>
<img width="1849" height="928" alt="TridiumScreenShotDark" src="https://github.com/user-attachments/assets/0c35ff74-ad92-42ba-8427-36860b9b9c33" />

https://github.com/user-attachments/assets/53f32f6c-01aa-46b6-8b29-28ce7284153f
> *Note: This demo video was from before the RHI branch was created.*

---

## Features

- **Editor:** Editor for asset management and scene building inspired by engines like UE5.
- **Physics:** Integrated physics system for 3D simulations using JoltPhysics.
- **ECS:** Efficient Entity Component System for organizing game objects using EnTT.
- **C++ Runtime Reflection:** Dynamic runtime type information for classes, properties, and serialization built on top of EnTT. 
- **Serialization:** Saving/loading game data.
- **Graphics:** PBR rendering pipeline built with render-graphs and support for advanced materials.
- **RHI (Rendering Hardware Interface):** Abstracted graphics API layer supporting OpenGL and DirectX 12 (Vulkan support coming soon).

## TODO
Developing the RHI and integrating  it into the engine is a big job and requires a rewrite of many systems.
- [ ] **RHI** - Work in Progress ⚙️
  - [ ] **Features** - Work in Progress ⚙️ 
    - [x] **Core API** - Done ✅
    - [ ] **Compute Shaders** - Not Started ❌
    - [ ] **Raytracing Support** - Not Started ❌
  - [ ] **Backends** - Work in Progress ⚙️
    - [x] **DirectX12 support** - Done ✅
    - [x] **OpenGL support** - Done ✅
    - [ ] **Vulkan support** - Work in Progress ⚙️
    - [ ] **Metal support** - Not Started ❌
    - [ ] **DirectX11 support** - Not Started ❌
- [ ] **New Asset System** - Work in Progress ⚙️
  - [x] **Core API** - Done ✅  
- [ ] **New Scene Renderer** - Work in Progress ⚙️
  - [x] **GBuffer** - Done ✅
  - [ ] **Shadows** - Not Started ❌
- [ ] **This TODO list** - Work in Progress ⚙️
---

## Getting Started
> [!NOTE]
> **Windows** is the only platform that is currently tested.

> [!NOTE]
> **Visual Studio 2022** is required.

**1.** Clone the repository:
```bash
# This will clone the RHI branch repo into "Your/Path/To/Tridium"
git clone -b RHI --single-branch https://github.com/AsherFarag/Tridium.git

# Enter the new Tridium folder
cd Tridium
```

**2.** Run the setup script to generate the Visual Studio 2022 solution:
> *Note: Downloading and building the dependencies can take a while.*
```bash
# Invokes the Setup.bat script which:
# 1. Setup dependencies
# 2. Validated your Vulkan SDK (if invalid, it gives you the option to install it)
# 3. Generates the Visual Studio projects with premake
Setup
```

**3.** Open the generated .sln file in Visual Studio 2022 and build the project.
> ⚠️ **Important:** Set the build config to either Debug-Editor or Release-Editor. The other configurations are unstable and may not build.

**Optional: 4.** Adds custom Visual Studio Item templates (These can be removed with UninstallTemplates.bat)
```bash
InstallTemplates
```

## Credits & Dependencies

Tridium uses a number of open-source libraries and tools. We gratefully acknowledge the authors and contributors of these projects:

- **[Assimp](https://www.assimp.org/)** – Open Asset Import Library for importing 3D models.  
- **[EnTT](https://github.com/skypjack/entt)** – Fast and flexible Entity Component System (ECS) for C++.  
- **[GLAD](https://glad.dav1d.de/)** – OpenGL loader library.  
- **[GLFW](https://www.glfw.org/)** – Library for creating windows, contexts, and handling input.  
- **[GLM](https://github.com/g-truc/glm)** – Header-only C++ mathematics library for graphics software.  
- **[ImGui](https://github.com/ocornut/imgui)** – Immediate mode GUI library.  
- **[ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)** – 3D gizmo manipulator for ImGui.  
- **[ImTextEdit](https://github.com/ChemistAion/ImTextEdit)** – Script editor for ImGui.
- **[Jolt Physics](https://github.com/jrouwe/JoltPhysics)** – Physics engine for rigid body dynamics.  
- **[Lua](https://www.lua.org/)** – Lightweight scripting language.  
- **[Refl-Cpp](https://github.com/veselink1/refl-cpp)** – C++ reflection library.  
- **[Sol2](https://github.com/ThePhD/sol2)** – C++ library binding for Lua.  
- **[spdlog](https://github.com/gabime/spdlog)** – Fast C++ logging library.  
- **[SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross)** – Shader cross-compiler for SPIR-V to other shading languages.  
- **[yaml-cpp](https://github.com/jbeder/yaml-cpp)** – YAML parser and emitter in C++.  
- **[zlib](https://zlib.net/)** – Compression library for data streams.  

## License
Code released under
[the Apache 2.0 license](https://github.com/AsherFarag/Tridium/blob/main/LICENSE).<br/>
