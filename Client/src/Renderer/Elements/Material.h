#pragma once

class Material
{
public:
	Material();
	void Set(ID3D12GraphicsCommandList* command_list) const;

	string   name              = "defaultMaterial";
	string   diffuse_map_name  = "defaultDiffuseMap";
	string   normal_map_name   = "defaultNormalMap";
	string   specular_map_name = "defaultSpecularMap";
	XMFLOAT3 fresnel_factor    = XMFLOAT3(0.1f, 0.1f, 0.1f);
};
