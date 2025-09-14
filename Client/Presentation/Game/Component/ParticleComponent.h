#pragma once
#include "Presentation/Game/Component/Component.h"

struct ParticlePropertiesPack;

class ParticleComponent : public Component
{
public:
    ParticleComponent() = delete;
    ParticleComponent(Object* pObject, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
    ~ParticleComponent() override;

    void Update(float fTimeElapsed) override;
    void Render(ID3D12GraphicsCommandList* pd3dCommandList) override;

    void Initialize(ParticlePropertiesPack& ppp);

protected:
    void CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle);
    void SetCBVGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);

private:
    // Render
    ID3D12Resource*          m_pd3dVertexBuffer;
    ID3D12Resource*          m_pd3dVertexUploadBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;

    // WorldTransform
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorHandle;
    ID3D12Resource*             m_pd3dCBResource          = nullptr;
    XMFLOAT4X4*                 m_pCBMappedWorldTransform = nullptr;

    // Particle Properties
    float    m_fLifetime;
    XMFLOAT3 m_xmf3Direction;
    float    m_fSpeed;
    float    m_fGravityModifier;
    float    m_fGravity;
    float    m_fSize;
    bool     m_bIsBilboard;

    TransformComponent* transform;
};
