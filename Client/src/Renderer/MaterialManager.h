#pragma once
#include "Elements/Material.h"

class MaterialManager
{
public:
	void Initialize()
	{
		materials_.clear();
		current_material_name_ = "";

		const Material default_material;
		AddMaterial(default_material);
	}

	void AddMaterial(const Material& new_material)
	{
		if (materials_.contains(new_material.name.c_str())) throw std::logic_error("Tried to add an already existing material.");
		materials_[new_material.name.c_str()] = new_material;
	}

	void SetMaterial(const char* material_name_to_set, ID3D12GraphicsCommandList* command_list)
	{
		if (current_material_name_ == material_name_to_set) return;
		materials_[material_name_to_set].Set(command_list);
	}

private:
	unordered_map<string, Material> materials_;
	string                          current_material_name_;
};
