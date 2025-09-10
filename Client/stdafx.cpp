#include "stdafx.h"
#include "Texture.h"
#include "Model.h"
#include "Material.h"
#include "Animation.h"
#include "Scene.h"

TextureManager g_TextureMng;
ModelManager g_ModelMng;
MaterialManager g_MaterialMng;
AnimationManager g_AnimMng;
int gTestInt = 2;

UINT gnCbvSrvDescriptorIncrementSize = 0;
Scene* g_pCurrScene = nullptr;
