#include "pch.h"
#include "Screen.h"

#include "Presentation/Renderer/DirectX/DirectXMethods.h"
#include "Presentation/Renderer/Elements/Mesh.h"

Screen::Screen(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle, float width, float height)
{
    m_pScreenMesh = new Mesh(pd3dDevice, pd3dCommandList, width, height);

    UINT ncbElementBytes = sizeof(XMFLOAT4X4) + 255 & ~255;

    m_pd3dCBResource = CreateBufferResource(pd3dDevice, pd3dCommandList, nullptr, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

    D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

    if (nullptr != m_pd3dCBResource)
    {
        m_pd3dCBResource->Map(0, nullptr, (void**)&m_pCBMappedTransform);
        D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
        d3dCBVDesc.SizeInBytes = ncbElementBytes;
        d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
        pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorStartHandle);

        d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
    }

    m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorStartHandle;
    d3dCbvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}

Screen::~Screen()
{
    if (m_pScreenMesh) delete m_pScreenMesh;
    if (m_pd3dCBResource)
    {
        m_pd3dCBResource->Unmap(0, nullptr);
        m_pd3dCBResource->Release();
    }
    if (m_pCBMappedTransform) delete m_pCBMappedTransform;
}

void Screen::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
    pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_OBJECTS, m_d3dCbvGPUDescriptorHandle);
    UINT ncbElementBytes = sizeof(XMFLOAT4X4) + 255 & ~255;
    memset(m_pCBMappedTransform, NULL, ncbElementBytes);
    XMStoreFloat4x4(m_pCBMappedTransform, XMMatrixTranspose(XMMatrixIdentity()));

    m_pScreenMesh->Render(pd3dCommandList);
}