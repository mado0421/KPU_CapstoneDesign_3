# KPU Capstone Design 3 - Client

## Project Overview

This is a **C++20 Windows Game Client** developed as part of a Capstone Design project. It utilizes **DirectX 12** for rendering and features a custom Component-Entity architecture.

## Architecture

The project follows a standard game engine structure:

### Core Systems
*   **Entry Point (`src/Core/Client.cpp`):** Contains `WinMain`, initializes the application window, and starts the message loop.
*   **Framework (`src/Core/Framework.h`):** The central engine class. It manages the `Scene`, handles the main game loop (`Update`, `Render`), and manages DirectX 12 resources (Device, SwapChain, CommandQueue).
*   **Scene Management (`src/Core/Scene.h`):** Manages the lifecycle of game objects, lights, and rendering passes.

### Entity-Component System (ECS)
The game logic is built around Objects and Components:
*   **Object (`src/Core/Object.h`):** The base game entity that exists in a scene.
*   **Component (`src/Core/Component.h`):** The base class for logic attached to objects. Key components include:
    *   `TransformComponent`: Position, rotation, scale.
    *   `MeshRendererComponent` / `SkinnedMeshRendererComponent`: Rendering.
    *   `ColliderComponent` / `RigidbodyComponent`: Physics.
    *   `AnimatorComponent`: Animation control.

### Rendering (DirectX 12)
*   **Shaders:** HLSL shaders are located in `Resources/Shaders/`.
*   **Resources:** Textures (`.dds`) and meshes (`.mm`) are loaded from `Resources/`.
*   **Pipeline:** The `Scene` class orchestrates the rendering pipeline, managing Root Signatures, Descriptor Heaps, and Pipeline State Objects (PSOs).

## Build & Run

### Prerequisites
*   Visual Studio 2019 or later (v142/v145 toolset).
*   Windows SDK 10.0.

### Building
The project is configured as a Visual Studio Solution (`WindowsProject1.sln`).
1.  Open `WindowsProject1.vcxproj` (or the Solution file if available).
2.  Select the configuration (e.g., `Debug` or `Release`) and platform (`x64`).
3.  Build the solution (Ctrl+Shift+B).

### Directory Structure
*   **`src/`**: Source code, organized by module (`Core`, `Components`, `Renderer`, `Graphics`, `Character`, `Utils`).
*   **`Resources/`**: Game assets including textures, models, data files (`.txt`), and shaders (`.hlsl`).
*   **`pch.h` / `pch.cpp`**: Precompiled header files.

## Coding Conventions
*   **Language Standard:** C++20.
*   **Naming:** PascalCase for classes and functions.
*   **Member Variables:** `m_` prefix (e.g., `m_pObject`, `m_bEnabled`).
*   **Pointers:** Often prefixed with `p` (e.g., `pObject`).
*   **Macros:** Used for constants (e.g., `NUMSWAPCHAINBUFFER`).
