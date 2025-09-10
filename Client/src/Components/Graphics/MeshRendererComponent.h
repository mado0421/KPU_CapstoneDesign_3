#pragma once
#include "../../Core/Component.h"


class MeshRendererComponent : public Component
{
public:
    MeshRendererComponent() = delete;
    MeshRendererComponent(Object* pObject, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
    ~MeshRendererComponent() override;

    void Render(ID3D12GraphicsCommandList* pd3dCommandList) override;

    void SetModelByName(const char* strModelName);
    void SetMaterialByName(const char* strMaterialName);

protected:
    void CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle);
    void SetCBVGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);

    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorHandle;
    ID3D12Resource*             m_pd3dCBResource          = nullptr;
    XMFLOAT4X4*                 m_pCBMappedWorldTransform = nullptr;
    string                      m_strModelName            = "";
    string                      m_strMaterialName         = "";
};
