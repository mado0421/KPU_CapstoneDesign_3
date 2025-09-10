#pragma once
#include "../../pch.h"

class Object;
class Screen;
class Camera;
class RenderToTextureClass;
class Framework;
class Texture;
class LightManager;
class TextureManager;

struct CB_PASS_INFO
{
    XMFLOAT4X4 m_xmf4x4CameraView;
    XMFLOAT4X4 m_xmf4x4CameraProjection;
    XMFLOAT4X4 m_xmf4x4CameraViewInv;
    XMFLOAT4X4 m_xmf4x4CameraProjectionInv;
    XMFLOAT4X4 m_xmf4x4TextureTransform;
    XMFLOAT3   m_xmf3CameraPosition;
    float      m_xmfCurrentTime;
};

struct MY_ENV_OBJECT_DATA
{
    string   strMeshName;
    string   strMatName;
    XMFLOAT3 xmf3Position;
    XMFLOAT4 xmf4Rotation;
};

struct MY_COLLIDER_OBJECT_DATA
{
    XMFLOAT3 xmf3Position;
    XMFLOAT3 xmf3Extents;
    XMFLOAT4 xmf4Rotation;
};

namespace LoadMy
{
    vector<string>                  Split(istringstream& ss, char delim);
    vector<MY_ENV_OBJECT_DATA>      LoadEnvMeshList(const char* path);
    vector<MY_COLLIDER_OBJECT_DATA> LoadColliderList(const char* path);
}

enum class RenderGroup
{
    OBJECT,
    ANIMATED,
    PARTICLE,
    EFFECT
};

class Scene
{
protected:
    ID3D12RootSignature*       m_pd3dRootSignature;
    ID3D12Device*              m_pd3dDevice;
    ID3D12GraphicsCommandList* m_pd3dCommandList;
    ID3D12DescriptorHeap*      m_pd3dCbvSrvDescriptorHeap;

    D3D12_CPU_DESCRIPTOR_HANDLE m_d3dCbvCPUDescriptorStartHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorStartHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE m_d3dSrvCPUDescriptorStartHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dSrvGPUDescriptorStartHandle;


    vector<Object*> m_vecObject;

    // for Render
    vector<Object*> m_vecNonAnimObjectRenderGroup;
    vector<Object*> m_vecAnimObjectRenderGroup;
    vector<Object*> m_vecEffectRenderGroup;
    vector<Object*> m_vecUIRenderGroup;

    // for Particle
    vector<Object*> m_vecParticleEmitter;

    // for Camera
    Object* m_pCameraObject = nullptr;

    vector<Screen*>                             m_vecScreenObject;
    unordered_map<string, ID3D12PipelineState*> m_uomPipelineStates;

    LightManager* m_LightMng;

    ID3D12Resource*             m_pd3duabHDRAvgLum;
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUuabHDRAvgLumHandle;
    ID3D12Resource*             m_pd3dcbPassInfo;
    CB_PASS_INFO*               m_pcbMappedPassInfo;
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUPassInfoHandle;

    Framework* m_pFramework;
    Camera*    m_pCamera;
    float      m_fCurrentTime = 0;

public:
    vector<Object*> m_vecParticlePool;

    int          eventCount    = 0; //enemyDown
    bool         test          = false;
    int          startEndState = 0; // 0: main, 1: start, 2: end
    virtual void Init(Framework* pFramework, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

    virtual void CheckCollsion();
    virtual void SolveConstraint();
    virtual void Input(UCHAR* pKeyBuffer);
    virtual void Update(float fTimeElapsed);
    virtual void Render(D3D12_CPU_DESCRIPTOR_HANDLE hBckBufRtv, D3D12_CPU_DESCRIPTOR_HANDLE hBckBufDsv);

    virtual void Release();

    void Clear();
    void Victory();
    void Defeat();

    void    AddObject(Object* pObject, RenderGroup renderGroup);
    void    DeleteObject(Object* pObject);
    Object* FindObjectByName(const char*);

protected:
    virtual ID3D12RootSignature* CreateRootSignature();
    virtual void                 CreateDescriptorHeap();

    virtual void CreateCBV() {}

    virtual void CreateSRV() {}

    virtual void CreatePSO();

    void CreatePassInfoShaderResource();
    void UpdatePassInfoAboutCamera();

    void BuildObject();

    /*==============================================================================
        * For Test!!
        *
        ==============================================================================*/
    void ReloadLight();

    void LoadLevelEnvironment();

    void CreateEnvObject(const char* strModelName, const char* strMaterialName, XMFLOAT3 pos, XMFLOAT4 rot);
    void CreateEnvObject(MY_ENV_OBJECT_DATA objData);

    void CreateCollider(MY_COLLIDER_OBJECT_DATA colData);

    void CreateTargetBoard(const char* strName, XMFLOAT3 position, XMFLOAT3 rotationAngle, bool initialStateDied);
    void CreateDoor(const char* strName, XMFLOAT3 position, XMFLOAT3 rotationAngle, bool isOpen);
};
