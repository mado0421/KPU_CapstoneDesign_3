#include "pch.h"
#include "src/Scene/Scene.h"
#include "src/Renderer/AnimationManager.h"
#include "src/Renderer/MaterialManager.h"
#include "src/Renderer/Elements/Animation.h"
#include "src/Renderer/Elements/Model.h"
#include "src/Renderer/Elements/Texture.h"

AnimationManager g_AnimMng;
MaterialManager  g_MaterialMng;
TextureManager   g_texture_manager;
ModelManager     g_ModelMng;
UINT             gnCbvSrvDescriptorIncrementSize = 0;

int    gTestInt     = 2;
Scene* g_pCurrScene = nullptr;
