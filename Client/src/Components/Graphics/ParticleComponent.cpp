#include "pch.h"
#include "../../Core/Components.h"

#include "src/Renderer/Elements/Vertex.h"

ParticleComponent::ParticleComponent(Object*                      pObject,
									 ID3D12Device*                pd3dDevice,
									 ID3D12GraphicsCommandList*   pd3dCommandList,
									 D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
									 D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle) : Component(pObject)
{
    auto pVertices = new Vertex();

    pVertices->position = XMFLOAT3(0, 0, 0);

    m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, sizeof(Vertex), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

    m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
    m_d3dVertexBufferView.StrideInBytes  = sizeof(Vertex);
    m_d3dVertexBufferView.SizeInBytes    = sizeof(Vertex);


    UINT ncbElementBytes = sizeof(XMFLOAT4X4) + 255 & ~255;

    m_pd3dCBResource = CreateBufferResource(pd3dDevice, pd3dCommandList, nullptr, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

    CreateConstantBufferView(pd3dDevice, d3dCbvCPUDescriptorStartHandle);
    SetCBVGpuHandle(d3dCbvGPUDescriptorStartHandle);
}

ParticleComponent::~ParticleComponent()
{
    if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
    if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
}

void ParticleComponent::Update(float fTimeElapsed)
{
    if (!m_bEnabled) return;

    m_fLifetime -= fTimeElapsed;
    if (0 >= m_fLifetime)
    {
        m_pObject->SetActive(false);
        SetActive(false);
    }
    m_fGravity += 9.8f * m_fGravityModifier * fTimeElapsed;
    XMFLOAT3 Velocity = Vector3::Multiply(m_fSpeed, m_xmf3Direction);
    XMFLOAT3 G        = Vector3::Multiply(m_fGravity, XMFLOAT3(0, -1.0f, 0));

    Velocity = Vector3::Add(G, Velocity);
    Velocity = Vector3::Multiply(fTimeElapsed, Velocity);

    transform->SetPosition(Vector3::Add(transform->GetPosition(), Velocity));
}

void ParticleComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
    pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_OBJECTS, m_d3dCbvGPUDescriptorHandle);
    UINT ncbElementBytes = sizeof(XMFLOAT4X4) + 255 & ~255;
    memset(m_pCBMappedWorldTransform, NULL, ncbElementBytes);

    // ��¥�� ���庯ȯ �ʿ� ���� ���� ��ġ�� ������ �Ǵµ�
    // ������ ��Ʈ�ñ״�ó �� �ٲٰ� �ϱ�� �ȴ�!
    // �״ϱ� ������ȯ��Ŀ� _44�� size�� �ٲ㼭 ������
    // VS���� input.positionW = float(_41, _42, _43) �ϰ�
    // GS���� size�� _44�� ����~ �Ϻ�~
    // 
    // ��ƼŬ�� �����尡 �ƴ� ��Ȳ�� ����ؾ� ��
    // input.direction = float(_11, _12, _13)
    // isBilboard = bool(_14)
    // �� ����
    // 
    // 
    // 
    // 
    //


    XMFLOAT4X4 xmf4x4Temp;
    XMStoreFloat4x4(&xmf4x4Temp, transform->GetWorldTransform());
    xmf4x4Temp._44 = m_fSize;
    xmf4x4Temp._11 = m_xmf3Direction.x;
    xmf4x4Temp._12 = m_xmf3Direction.y;
    xmf4x4Temp._13 = m_xmf3Direction.z;
    xmf4x4Temp._14 = m_bIsBilboard
                         ? 0.0f
                         : 1.0f;

    XMStoreFloat4x4(m_pCBMappedWorldTransform, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

    pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferView);
    pd3dCommandList->DrawInstanced(1, 1, 0, 0);
}

void ParticleComponent::Initialize(ParticlePropertiesPack& ppp)
{
    transform = m_pObject->FindComponent<TransformComponent>();
    transform->SetPosition(ppp.currPos);
    m_xmf3Direction    = ppp.direction;
    m_fLifetime        = ppp.lifetime;
    m_fSpeed           = ppp.speed;
    m_fGravityModifier = ppp.gravity;
    m_fSize            = ppp.size;
    m_fGravity         = 0;
    m_bIsBilboard      = ppp.isBilboard;
    m_pObject->SetActive(true);
    SetActive(true);
}

void ParticleComponent::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle)
{
    D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
    UINT                            ncbElementBytes = sizeof(XMFLOAT4X4) + 255 & ~255;

    if (nullptr != m_pd3dCBResource)
    {
        m_pd3dCBResource->Map(0, nullptr, (void**)&m_pCBMappedWorldTransform);
        D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
        d3dCBVDesc.SizeInBytes                         = ncbElementBytes;
        d3dCBVDesc.BufferLocation                      = d3dGpuVirtualAddress;
        pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorStartHandle);

        d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
    }
}

void ParticleComponent::SetCBVGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
{
    m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorStartHandle;
    d3dCbvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}
