# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a KPU Capstone Design project implementing a 3D multiplayer third-person shooter (TPS) game using DirectX 12 and C++. The project consists of a client-server architecture with advanced graphics features.

### Architecture

The project is organized into two main components:

**Client (`Client/` directory)**

- DirectX 12-based 3D game engine with advanced rendering features
- Component-based architecture with Object-Component system
- Entry point: `Client/Presentation/Game/Core/Client.cpp` (wWinMain function) and `Client/Presentation/Game/Core/Framework.h`
- Core classes: `Object`, `Component`, `Scene`, `Framework`
- Organized source structure:
    - `Presentation/Game/Core/` - Framework, Client main entry point
    - `Presentation/Game/Component/` - All game components (Animation, Controllers, Graphics, Physics, UI, Scripts)
    - `Presentation/Game/` - Character-related classes and main game objects
    - `Presentation/Renderer/Elements/` - Rendering elements (Camera, Light, Material, Mesh, Model, Texture, Vertex)
    - `Presentation/Renderer/DirectX/` - DirectX utilities and methods
    - `Presentation/ResourceLoader/` - Resource loading and importing
    - `Presentation/Game/Manager/` - Animation, Material, and Texture managers
    - `Presentation/Game/Utility/` - Utility classes (Timer)
    - `Presentation/Game/Input/` - Input handling (KeyCode)
- Precompiled header: `pch.h` and `pch.cpp`

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

- Vertex shaders: `Client/shaders/VertexShader.hlsl`
- Pixel shaders: `Client/shaders/PixelShader.hlsl`
- Geometry shaders: `Client/shaders/GeometryShader.hlsl`
- Compute shaders: `Client/shaders/ComputeShader.hlsl`
- Common headers: `Client/shaders/Header.hlsli`, `Client/shaders/Light.hlsli`

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
- `Client/Assets/` - Game assets (models, textures, etc.)
- `Client/shaders/` - All HLSL shader files
- `Client/Presentation/` - Organized source code structure
- `Server/document/` - Server documentation and TODO lists
- `Server/작업일지/` - Development work logs and screenshots
- `작업일지/` - Weekly development logs (docx files)
- `포트폴리오/` - Portfolio documents and presentations
- `졸작지도/` - Capstone project thesis and meeting notes
- `릴리즈/` - Release builds and distributions
- `DevelopDailyLog/` - Additional development documentation

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
- Precompiled headers: `pch.h` and `pch.cpp` (replaced `stdafx.h`)

### Project Refactoring Status

- Code reorganized into structured `Presentation/` directory with logical separation
- Components categorized by functionality (Animation, Controllers, Graphics, Physics, UI, Scripts)
- Core game classes moved to `Presentation/Game/Core/`
- Rendering system organized under `Presentation/Renderer/` with Elements and DirectX subdirectories
- Resource management moved to `Presentation/ResourceLoader/`
- Managers organized under `Presentation/Game/Manager/`
- Utilities and input handling properly categorized
- Shaders moved to dedicated `shaders/` directory
- Precompiled headers updated from `stdafx.h` to `pch.h`
- Many legacy files removed as part of cleanup (as seen in git status)

## Client Header Files Structure Map

이 섹션은 모든 헤더 파일의 정확한 위치를 문서화하여 include 경로 작업 시 참조용으로 사용합니다.

### 프로젝트 루트 레벨
```
pch.h                    - 프리컴파일드 헤더
Resource.h               - 리소스 정의
targetver.h              - 타겟 버전 정의
```

### Presentation/Game/Core/ (핵심 게임 엔진)
```
Client.h                 - 메인 클라이언트 엔트리 포인트
Framework.h              - 게임 프레임워크
Scene.h                  - 씬 관리
```

### Presentation/Game/Component/ (모든 컴포넌트)
```
Component.h              - 기본 컴포넌트 클래스 ⭐
Components.h             - 모든 컴포넌트 포함 헤더
IComponent.h             - 컴포넌트 인터페이스

// 그래픽스 컴포넌트
CameraComponent.h
EffectComponent.h
MeshRendererComponent.h
ParticleComponent.h
ParticleEmitterComponent.h

// 애니메이션 컴포넌트
AnimatorComponent.h
SkinnedMeshRendererComponent.h

// 물리 컴포넌트
ColliderComponent.h
RigidbodyComponent.h

// 컨트롤러 컴포넌트
HumanoidControllerComponent.h
TargetBoardControllerComponent.h
WeaponControllerComponent.h

// UI 컴포넌트
TextRendererComponent.h
TextUI.h

// 스크립트 컴포넌트
DoorComponent.h
EnemyDownCounter.h
EventComponent.h
GameManagerComponent.h

// 기타 컴포넌트
InputManagerComponent.h
TransformComponent.h
```

### Presentation/Game/ (게임 오브젝트)
```
Character.h              - 캐릭터 클래스
Object.h                 - 게임 오브젝트 기본 클래스
```

### Presentation/Game/Manager/ (매니저 클래스들)
```
AnimationManager.h
MaterialManager.h
TextureManager.h
```

### Presentation/Game/Utility/ (유틸리티)
```
Timer.h
```

### Presentation/Game/Input/ (입력 처리)
```
KeyCode.h
```

### Presentation/Renderer/ (렌더링 시스템)
```
PipelineStateObject.h
```

### Presentation/Renderer/Elements/ (렌더링 요소들)
```
Animation.h
BoneMask.h
Camera.h
Light.h
Material.h
Mesh.h
Model.h
Texture.h
Vertex.h
```

### Presentation/Renderer/DirectX/ (DirectX 유틸리티)
```
DirectXMethods.h
d3dUtil.h
d3dx12.h
```

### Presentation/ResourceLoader/ (리소스 로더)
```
DDSTextureLoader12.h
Importer.h
```

### Presentation/ (기타)
```
MathHelper.h
```

### Resources/ (리소스 관련)
```
fontReader/FontReader.h
```

## Important Notes
- ⭐ 표시된 파일들은 자주 참조되는 핵심 파일
- Component.h는 Presentation/Game/Component/ 폴더에 위치 (Core 폴더 아님)
- 모든 include 경로는 프로젝트 루트 기준 절대 경로로 작성해야 함
- 예: `#include "Presentation/Game/Component/Component.h"`

# important-instruction-reminders

## Include 경로 작업 지침

### 필수 사전 확인 사항
1. **헤더 파일 구조도 참조**: 위의 "Client Header Files Structure Map"을 반드시 확인
2. **실제 파일 위치 검증**: vcxproj 파일에서 실제 파일 경로 확인
3. **추측 금지**: 파일명만 보고 경로를 추측하지 말 것

### Include 경로 수정 원칙
- 모든 include는 프로젝트 루트 기준 절대 경로 사용
- 상대 경로 (`../`, `../../`) 사용 금지
- 예: `"Presentation/Game/Component/Component.h"` (O)
- 예: `"../../Component/Component.h"` (X)

### 구조도 갱신 워크플로우
1. 새 파일 추가 시: 즉시 구조도에 추가
2. 파일 이동/삭제 시: 즉시 구조도에서 수정/제거
3. 대규모 리팩토링 후: 전체 구조도 재검증 및 업데이트
4. 매 작업 완료 시: 구조도 정확성 점검

### 작업 전 체크리스트
- [ ] 구조도에서 대상 파일 위치 확인
- [ ] vcxproj에서 실제 경로 검증
- [ ] include 경로를 절대 경로로 작성
- [ ] 변경 후 구조도 업데이트 필요한지 확인

### 주요 실수 방지
- **Component.h**: `Presentation/Game/Component/Component.h` (Core 폴더 아님!)
- **Framework.h**: `Presentation/Game/Core/Framework.h`
- **Object.h**: `Presentation/Game/Object.h` (Core 폴더 아님!)
- **Character.h**: `Presentation/Game/Character.h`