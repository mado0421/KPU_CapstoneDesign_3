#include "pch.h"
#include "src/Core/Scene.h"
#include "src/Graphics/Material.h"
#include "src/Resources/Animation.h"
#include "src/Resources/Model.h"
#include "src/Resources/Texture.h"

AnimationManager g_AnimMng;
MaterialManager  g_MaterialMng;
TextureManager   g_TextureMng;
ModelManager     g_ModelMng;
UINT             gnCbvSrvDescriptorIncrementSize = 0;

int    gTestInt     = 2;
Scene* g_pCurrScene = nullptr;
