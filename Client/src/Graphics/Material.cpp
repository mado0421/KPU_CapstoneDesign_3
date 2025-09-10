#include "pch.h"
#include "Material.h"

Material::Material() : matName("defaultMaterial"), diffuseMap("defaultDiffuseMap"), normalMap("defaultNormalMap"), specularMap("defaultSpecularMap"), fresnelFactor(XMFLOAT3(0.1f, 0.1f, 0.1f)) {}

void Material::Set(ID3D12GraphicsCommandList* pd3dCommandList)
{
    g_TextureMng.UseForShaderResource(diffuseMap.c_str(), pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
    g_TextureMng.UseForShaderResource(normalMap.c_str(), pd3dCommandList, ROOTSIGNATURE_NORMAL_TEXTURE);
    g_TextureMng.UseForShaderResource(specularMap.c_str(), pd3dCommandList, ROOTSIGNATURE_DEPTH_TEXTURE);
}
