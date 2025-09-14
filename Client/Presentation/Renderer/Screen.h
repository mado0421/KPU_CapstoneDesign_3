#pragma once

class Mesh;

class Screen
{
public:
    Screen() = delete;
    Screen(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle, float width, float height);
    ~Screen();

    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

private:
    Mesh* m_pScreenMesh = nullptr;
    ID3D12Resource* m_pd3dCBResource = nullptr;
    XMFLOAT4X4* m_pCBMappedTransform = nullptr;
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorHandle;
};
