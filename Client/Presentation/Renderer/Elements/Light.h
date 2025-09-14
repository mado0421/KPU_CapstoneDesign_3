#pragma once

constexpr UINT SpotLightShadowIdxIncrement        = 1;
constexpr UINT PointLightShadowIdxIncrement       = 6;
constexpr UINT DirectionalLightShadowIdxIncrement = 3;
constexpr UINT MAXSHADOWIDX                       = 64;

using LightType = enum LIGHTTYPE
{
    LIGHT_NONE,
    LIGHT_POINT       = static_cast<UINT>(1),
    LIGHT_SPOT        = static_cast<UINT>(2),
    LIGHT_DIRECTIONAL = static_cast<UINT>(3)
};

struct CB_LIGHT_INFO
{
    XMFLOAT4X4 m_xmf4x4ViewProj[6];
    XMFLOAT3   xmf3Color;
    float      fFalloffStart;
    XMFLOAT3   xmf3Direction;
    float      fFalloffEnd;
    XMFLOAT3   xmf3Position;
    float      fSpotPower;
    UINT       uType;
    bool       bIsShadow;
    //bool		padding0;
    //bool		padding1;
    //bool		padding2;
    ////UINT		padding3;
    ////UINT		padding4;
    //XMFLOAT4X4	padding3;
};

struct LIGHT_DESC
{
    LightType lightType     = LIGHT_NONE;
    XMFLOAT3  xmf3Color     = XMFLOAT3(0, 0, 0);
    XMFLOAT3  xmf3Direction = XMFLOAT3(0, 0, 0);
    XMFLOAT3  xmf3Position  = XMFLOAT3(0, 0, 0);
    XMFLOAT2  xmf2Falloff   = XMFLOAT2(0, 0);
    float     fSpotPower    = 0;
    bool      bIsShadow     = false;
};

struct Light
{
    Light() = delete;
    Light(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle);

    ~Light()
    {
        if (m_pd3dCBResource)
        {
            m_pd3dCBResource->Unmap(0, nullptr);
            m_pd3dCBResource->Release();
        }
    }

    void      SetShaderResource(ID3D12GraphicsCommandList* pd3dCommandList);
    LightType GetLightType() { return m_uLightType; }

    void UpdateDirectionalLightOrthographicLH(XMFLOAT4X4 xmf4x4CameraView);

private:
    void CreateResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    void CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle);

public:
    LightType m_uLightType;
    bool      m_bIsEnable;

    XMFLOAT3 m_xmf3Color;
    XMFLOAT3 m_xmf3Position;
    XMFLOAT3 m_xmf3Direction;

    float m_fFalloffStart;
    float m_fFalloffEnd;
    float m_fSpotPower;

    bool       m_bIsShadow;
    XMFLOAT4X4 m_xmf4x4ViewProj[6];

    string m_shadowMapName;

    // For Directional Light Shadow
    float      m_fZ[4];
    XMFLOAT4   m_xmf4FrustumCorners[3][8];
    int        m_nCascade;
    XMFLOAT4X4 m_xmf4x4LightView;
    XMFLOAT4X4 m_xmf4x4LightViewInv;

private:
    ID3D12Resource*             m_pd3dCBResource;
    CB_LIGHT_INFO*              m_pCBMappedLight;
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescHandle;
};

class LightManager
{
public:
    UINT AddPointLight(LIGHT_DESC desc, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle);
    UINT AddDirectionalLight(LIGHT_DESC desc, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle, UINT nCascade = 3);
    UINT AddSpotLight(LIGHT_DESC desc, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescHandle);

    void TurnOn(UINT i) { m_vecLight[i]->m_bIsEnable = true; }
    void TurnOff(UINT i) { m_vecLight[i]->m_bIsEnable = false; }
    void SetPosition(XMFLOAT3 xmf3Position, UINT i) { m_vecLight[i]->m_xmf3Position = xmf3Position; }
    void SetDirection(XMFLOAT3 xmf3Direction, UINT i) { m_vecLight[i]->m_xmf3Direction = xmf3Direction; }
    void SetShadowMapName(const char* name, UINT i) { m_vecLight[i]->m_shadowMapName = name; }

    void SetShaderResource(ID3D12GraphicsCommandList* pd3dCommandList, UINT i) { m_vecLight[i]->SetShaderResource(pd3dCommandList); }

    void UpdateDirectionalLightOrthographicLH(XMFLOAT4X4 xmf4x4CameraView, UINT i) { m_vecLight[i]->UpdateDirectionalLightOrthographicLH(xmf4x4CameraView); }

    UINT      GetNumLight() { return static_cast<UINT>(m_vecLight.size()); }
    bool      GetIsShadow(UINT i) { return m_vecLight[i]->m_bIsShadow; }
    string    GetShadowMapName(UINT i) { return m_vecLight[i]->m_shadowMapName; }
    LightType GetLightType(UINT i) { return m_vecLight[i]->GetLightType(); }


    void DeleteAll() { m_vecLight.clear(); }

private:
    vector<Light*> m_vecLight;
};
