#pragma once

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include "src/Renderer/TextureManager.h"
#include "src/Renderer/Elements/Texture.h"

#ifdef DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif


#define WIN32_LEAN_AND_MEAN
#include <D3Dcompiler.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <algorithm>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <fstream>
#include <iostream>
#include <malloc.h>
#include <memory.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include <windows.h>
#include "targetver.h"
#include "src/IO/DDSTextureLoader12.h"
#include "src/Utils/MathHelper.h"



using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;


#define FRAME_BUFFER_WIDTH 1920
#define FRAME_BUFFER_HEIGHT 1080
#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))
#define MAXNUMCBV (1024 * 3)
#define MAXNUMSRV (128 * 2)
#define MAXNUMLIGHT 32
#define MAX_BONE_NUM 64
#define SHADOWMAPSIZE 2048
#define ROOTSIGNATURE_PASSCONSTANTS			0
#define ROOTSIGNATURE_OBJECTS				1
#define ROOTSIGNATURE_LIGHTS				2
#define ROOTSIGNATURE_COLOR_TEXTURE			3
#define ROOTSIGNATURE_NORMAL_TEXTURE		4
#define ROOTSIGNATURE_DEPTH_TEXTURE			5
#define ROOTSIGNATURE_SHADOW_TEXTURE		6
#define ROOTSIGNATURE_CUBE_TEXTURE			7
#define ROOTSIGNATURE_SHADOWARRAY_TEXTURE	8
#define ROOTSIGNATURE_ANIMTRANSFORM			9
#define ROOTSIGNATURE_POSTPROCESS_TEXTURE	10
#define ROOTSIGNATURE_HDRLUMBUFFER			11
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05


class ModelManager;
class MaterialManager;
class AnimationManager;
extern AnimationManager g_AnimMng;
extern MaterialManager  g_MaterialMng;
extern TextureManager   g_texture_manager;
extern ModelManager     g_ModelMng;
extern UINT             gnCbvSrvDescriptorIncrementSize;
extern int              gTestInt;

class Scene;
extern Scene* g_pCurrScene;