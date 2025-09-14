#include "pch.h"

#include "Presentation/Game/Manager/AnimationManager.h"
#include "Presentation/Game/Manager/MaterialManager.h"
#include "Presentation/Game/Manager/TextureManager.h"
#include "Presentation/Renderer/Elements/Model.h"

AnimationManager g_AnimMng;
MaterialManager  g_MaterialMng;
TextureManager   g_texture_manager;
ModelManager     g_ModelMng;
UINT             gnCbvSrvDescriptorIncrementSize = 0;

int    gTestInt     = 2;
Scene* g_pCurrScene = nullptr;
