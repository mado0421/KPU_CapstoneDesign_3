#pragma once
#include "Presentation/Game/Component/Component.h"

struct CB_BONE_INFO
{
    XMFLOAT4X4 arrxmf4x4Transform[MAX_BONE_NUM];
};

class SkinnedMeshRendererComponent : public Component
{
public:
    SkinnedMeshRendererComponent() = delete;
    SkinnedMeshRendererComponent(GameObject*                      pObject,
                                 ID3D12Device*                pd3dDevice,
                                 ID3D12GraphicsCommandList*   pd3dCommandList,
                                 D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
                                 D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
    ~SkinnedMeshRendererComponent() override;

    void Render(ID3D12GraphicsCommandList* pd3dCommandList) override;

    void SetModelByName(const char* strModelName);
    void SetMaterialByName(const char* strMaterialName);

protected:
    void CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle);
    void SetCBVGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);

    // for Render Mesh
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorHandleForMesh;
    ID3D12Resource*             m_pd3dCBResourceForMesh   = nullptr;
    XMFLOAT4X4*                 m_pCBMappedWorldTransform = nullptr;
    string                      m_strModelName            = "";
    string                      m_strMaterialName         = "";

    // for Upload Animation Transform to GPU
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorHandleForAnimation;
    ID3D12Resource*             m_pd3dCBResourceForAnimation = nullptr;
    CB_BONE_INFO*               m_pCBMappedBonesTransform    = nullptr;
};
