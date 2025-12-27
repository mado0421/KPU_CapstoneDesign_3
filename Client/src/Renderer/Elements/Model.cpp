#include "pch.h"
#include "Model.h"

#include "Mesh.h"

#include "../../IO/Importer.h"

void ModelManager::Initialize() { m_uomModel.clear(); }

void ModelManager::AddModel(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    MeshDataImporter  meshDataImporter;
    vector<MeshData> vecMeshData = meshDataImporter.Load(fileName);
    Model             model;

    for (int i = 0; i < vecMeshData.size(); i++)
    {
        auto pMesh = new Mesh(pd3dDevice, pd3dCommandList, vecMeshData[i]);
        model.push_back(pMesh);
    }

    m_uomModel[fileName] = model;
}

void ModelManager::AddFBXModel(const char* fileName, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
    MeshDataImporter  meshDataImporter;
    vector<MeshData> vecMeshData = meshDataImporter.FbxLoad(fileName);
    Model             model;

    for (int i = 0; i < vecMeshData.size(); i++)
    {
        auto pMesh = new Mesh(pd3dDevice, pd3dCommandList, vecMeshData[i]);
        model.push_back(pMesh);
    }

    m_uomModel[fileName] = model;
}

void ModelManager::Render(const char* modelName, ID3D12GraphicsCommandList* pd3dCommandList)
{
    assert(m_uomModel.contains(modelName) && "modelName占쏙옙 틀占싫거놂옙, 占쏙옙占쏙옙 model占쌉니댐옙\n");
    for_each(m_uomModel[modelName].begin(), m_uomModel[modelName].end(), [&](Mesh* mesh) { mesh->Render(pd3dCommandList); });
}
