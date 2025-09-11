#include "pch.h"
#include "Material.h"

Material::Material() = default;

void Material::Set(ID3D12GraphicsCommandList* command_list) const
{
    g_texture_manager.UseForShaderResource(diffuse_map_name.c_str(), command_list, ROOTSIGNATURE_COLOR_TEXTURE);
    g_texture_manager.UseForShaderResource(normal_map_name.c_str(), command_list, ROOTSIGNATURE_NORMAL_TEXTURE);
    g_texture_manager.UseForShaderResource(specular_map_name.c_str(), command_list, ROOTSIGNATURE_DEPTH_TEXTURE);
}
