# Tridium <img width="30" height="30" alt="EngineIcon" src="https://github.com/user-attachments/assets/7d278b18-99f9-4553-bbad-07164aab1872" /></br>

**Tridium** is a modern 3D game engine built in C++ with a focus on flexibility, performance, and editor-driven development. It features an integrated Editor, Physics, ECS (Entity Component System), runtime C++ reflection, serialization, physically-based rendering (PBR), and a dynamically-bound Rendering Hardware Interface (RHI) supporting **OpenGL** and **DirectX 12**.

> ⚠️ **Important:** This project is currently under active development. Expect bugs and incomplete features.

---

<img width="1831" height="937" alt="Screenshot 2025-10-09 114927" src="https://github.com/user-attachments/assets/2e178b90-1288-4094-8a0d-be0d8b66234b" /></br>

https://github.com/user-attachments/assets/53f32f6c-01aa-46b6-8b29-28ce7284153f
> *Note:* This demo video was from before the RHI branch was created.

---

## Features

- **Editor:** Full-featured game editor for asset management and scene building.  
- **Physics:** Integrated physics system for 3D simulations using JoltPhysics.
- **ECS:** Efficient Entity Component System for organizing game objects using EnTT.
- **C++ Runtime Reflection:** Dynamic runtime type information for classes, properties, and serialization built on top of EnTT. 
- **Serialization:** Easy-to-use system for saving/loading game data.  
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

1. Clone the repository:
```bash
# This will clone the RHI branch repo into "Your/Path/To/Tridium"
git clone -b RHI --single-branch https://github.com/AsherFarag/Tridium.git

# Enter the new Tridium folder
cd Tridium
```

2. Run the setup script to generate the Visual Studio 2022 solution:
```bash
# Invokes the Setup.bat script which:
# 1. Setup dependencies
# 2. Validated your Vulkan SDK (if invalid, it gives you the option to install it)
# 3. Generates the Visual Studio projects with premake
# 4. Adds custom Visual Studio Item templates (These can be removed with UninstallTemplates.bat)
Setup
```

3. Open the generated .sln file in Visual Studio 2022 and build the project.

## License
Code released under
[the MIT license](https://github.com/AsherFarag/Tridium/blob/main/LICENSE).<br/>
