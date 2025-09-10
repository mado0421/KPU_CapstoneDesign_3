#pragma once

class Mesh;
using Model = vector<Mesh*>;

class ModelManager
{
public:
	void Initialize();
	void AddModel(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void AddFBXModel(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(const char* modelName, ID3D12GraphicsCommandList* pd3dCommandList);
	bool IsAleadyExist(const char* name) { return m_uomModel.contains(name); }

private:
	unordered_map<string, Model> m_uomModel;
};
