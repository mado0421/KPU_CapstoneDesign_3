# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a KPU Capstone Design project implementing a 3D multiplayer third-person shooter (TPS) game using DirectX 12 and C++. The project consists of a client-server architecture with advanced graphics features.

### Architecture

The project is organized into two main components:

**Client (`Client/` directory)**
- DirectX 12-based 3D game engine with advanced rendering features
- Component-based architecture with Object-Component system
- Entry point: `Client/WindowsProject1.cpp` and `Client/Framework.h`
- Core classes: `Object`, `Component`, `Scene`, `Framework`

**Server (`Server/` directory)**  
- Multiple server implementations:
  - `Server/serverFrame/` - Main game server with character and client management
  - `Server/WindowsClient/` - Windows client connector with WSA module
  - `Server/SimpleTestClient/` - Basic test client

## Build Commands

### Client
```bash
# Build the main client application
msbuild "Client\WindowsProject1.vcxproj" /p:Configuration=Debug /p:Platform=x64
msbuild "Client\WindowsProject1.vcxproj" /p:Configuration=Release /p:Platform=x64

# Or build entire solution
msbuild "KPU_CapstoneDesign.sln" /p:Configuration=Debug /p:Platform=x64
```

### Server Components
```bash
# Build server frame
msbuild "Server\serverFrame\serverFrame.sln" /p:Configuration=Debug /p:Platform=x64

# Build Windows client connector
msbuild "Server\WindowsClient\WindowsClient.sln" /p:Configuration=Debug /p:Platform=x64

# Build simple test client
msbuild "Server\SimpleTestClient\SimpleTestClient.sln" /p:Configuration=Debug /p:Platform=x64
```

## Key Technologies and Features

### Graphics Engine (Client)
- **DirectX 12** with modern rendering pipeline
- **Deferred Shading** rendering path with GBuffer
- **Advanced Lighting**: Spot lights, point lights with cube shadow maps, directional lights with cascade shadow maps
- **Post-processing**: Bloom, HDR, blur effects using compute shaders
- **Animation System**: FBX mesh loading, skeletal animation, animation blending
- **Physics**: Collision detection and constraint solving
- **Particle Systems**: Particle emitters and effects

### Component System
The engine uses an Entity-Component system where:
- `Object` serves as the entity container
- `Component` is the base class for all components
- Common components include: `TransformComponent`, `MeshRendererComponent`, `ColliderComponent`, `AnimatorComponent`, etc.

### Shader Pipeline
- Vertex shaders: `Client/VertexShader.hlsl`
- Pixel shaders: `Client/PixelShader.hlsl`  
- Geometry shaders: `Client/GeometryShader.hlsl`
- Compute shaders: `Client/ComputeShader.hlsl`
- Common headers: `Client/Header.hlsli`, `Client/Light.hlsli`

### Networking (Server)
- WSA-based networking module (`ClientWsaModule`)
- Character and client management system
- Packet-based communication defined in `Server/serverFrame/serverFrame/Packets/packet.h`

## Development Setup

### Prerequisites
- Visual Studio 2019 or later with C++ support
- Windows 10 SDK
- DirectX 12 capable graphics hardware

### Project Configuration
- Platform: Windows x64
- Language: C++ with Unicode character set
- DirectX: Version 12
- Shader Model: 5.1 for x64 builds

### Key Directories
- `Client/Data/` - Contains asset data files (AssetsData.txt, LightData.txt, MaterialData.txt, ObjectData.txt)
- `Server/document/` - Server documentation and TODO lists
- `Server/작업일지/` - Development work logs and screenshots

## Game Controls
- **WASD** - Movement
- **Mouse** - Camera control
- **Left Click** - Shoot
- **ESC** - Exit program
- **M** - Enable mouse lock
- **N** - Disable mouse lock

## Implementation Notes

### Graphics Features Implemented
- FBX mesh rendering with Diffuse, Normal, and Roughness maps
- Multiple shadow mapping techniques (spot, point, directional with CSM)
- Component-based object system
- Collision detection with wall sliding
- Particle effects and emitters
- Animation blending system

### Known Development Areas
- Server position synchronization needs improvement
- Collision checking requires server-side validation
- Object management for non-player entities (targets, bullets) needs expansion
- Input handling uses key buffer approach between client and server

### Visual Studio Project Structure
- Main solution: `KPU_CapstoneDesign.sln`
- Client project: `Client/WindowsProject1.vcxproj` (project name: TempProjectName)  
- Server projects maintained as separate solutions in respective directories
- Precompiled headers enabled for x64 Debug builds (`stdafx.h`)