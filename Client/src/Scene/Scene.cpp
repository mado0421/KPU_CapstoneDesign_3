#include "pch.h"
#include "Scene.h"
#include <set>
#include "../Engine/ECS/Object.h"
#include "../Renderer/Pipeline/PipelineStateObject.h"
#include "../Graphics/Light.h"
#include "../IO/Importer.h"
#include "../Renderer/Elements/Model.h"
#include "../Renderer/Elements/Texture.h"

#include "../Engine/ECS/Components.h"

#include "../Renderer/DirectX/DirectXMethods.h"
#include "../Renderer/DirectX/d3dx12.h"

bool TEST_MOUSE_USABLE = true;

Scene::~Scene()
{
    Release();
}

void Scene::Init(Framework* framework, ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
    m_pd3dDevice        = device;
    m_pd3dCommandList   = command_list;
    m_pd3dRootSignature = CreateRootSignature();

    if (TEST_MOUSE_USABLE) SetCursorPos(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);
    ShowCursor(false);
    CreateDescriptorHeap();

    CreatePassInfoShaderResource();
    {
        int totalBckBufPixels = FRAME_BUFFER_WIDTH * 2;

        HRESULT hr = E_FAIL;


        D3D12_HEAP_PROPERTIES hp   = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC   desc = CD3DX12_RESOURCE_DESC::Buffer(totalBckBufPixels * sizeof(float), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

        hr = device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_pd3duabHDRAvgLum));

        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format                           = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension                    = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements               = totalBckBufPixels * sizeof(float);
        uavDesc.Buffer.StructureByteStride       = 1;
        uavDesc.Buffer.Flags                     = D3D12_BUFFER_UAV_FLAG_NONE;

        m_pd3dDevice->CreateUnorderedAccessView(m_pd3duabHDRAvgLum, nullptr, &uavDesc, m_d3dSrvCPUDescriptorStartHandle);
        m_d3dSrvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
        m_d3dCbvGPUuabHDRAvgLumHandle = m_d3dSrvGPUDescriptorStartHandle;
        m_d3dSrvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
    }
    
    g_texture_manager.Initialize(m_pd3dDevice);
    g_texture_manager.AddUnorderedAccessTexture("DownScaled", m_pd3dDevice, FRAME_BUFFER_WIDTH / 4, FRAME_BUFFER_HEIGHT / 4, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.AddUnorderedAccessTexture("Blur_Vertical", m_pd3dDevice, FRAME_BUFFER_WIDTH / 4, FRAME_BUFFER_HEIGHT / 4, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.AddUnorderedAccessTexture("Blur_Horizontal", m_pd3dDevice, FRAME_BUFFER_WIDTH / 4, FRAME_BUFFER_HEIGHT / 4, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.AddDepthBufferTexture("GBuffer_Depth", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.AddRenderTargetTexture("GBuffer_Color", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.AddRenderTargetTexture("GBuffer_Normal", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.AddRenderTargetTexture("Screen", m_pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

    g_texture_manager.LoadFromFile("defaultDiffuseMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.LoadFromFile("defaultNormalMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.LoadFromFile("defaultSpecularMap", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.LoadFromFile("titleImage_rescale", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.LoadFromFile("endImage_rescale", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);
    g_texture_manager.LoadFromFile("victory", m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

    MaterialDataImporter matDataImporter;
    matDataImporter.Load("MaterialData");


    g_ModelMng.Initialize();


    AssetListDataImporter assetImporter;
    assetImporter.Load(m_pd3dDevice, m_pd3dCommandList, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);


    BuildObject();

    auto tempScreen = new Screen(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle, 1.0f, 1.0f);
    m_vecScreenObject.push_back(tempScreen);

    LightDataImporter  lightDataImporter;
    vector<LIGHT_DESC> vecLightDesc = lightDataImporter.Load("Resources/LightData.txt");
    string             shadow("ShadowMap_");
    m_LightMng = new LightManager();
    for (int i = 0; i < vecLightDesc.size(); i++)
    {
        switch (vecLightDesc[i].lightType)
        {
        case LIGHT_POINT: m_LightMng->AddPointLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

            break;
        case LIGHT_DIRECTIONAL: m_LightMng->AddDirectionalLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle, 3);

            break;
        case LIGHT_SPOT: m_LightMng->AddSpotLight(vecLightDesc[i], m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);

            break;
        case LIGHT_NONE: default: break;
        }

        if (vecLightDesc[i].bIsShadow)
        {
            string temp = to_string(i);
            temp        = shadow + temp;
            switch (vecLightDesc[i].lightType)
            {
            case LIGHT_POINT: g_texture_manager.AddDepthBufferTextureCube(temp.c_str(), m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

                break;

            case LIGHT_SPOT: g_texture_manager.AddDepthBufferTexture(temp.c_str(), m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);

                break;
            case LIGHT_DIRECTIONAL: g_texture_manager.AddDepthBufferTextureArray(temp.c_str(), 3, m_pd3dDevice, SHADOWMAPSIZE, SHADOWMAPSIZE, m_d3dSrvCPUDescriptorStartHandle, m_d3dSrvGPUDescriptorStartHandle);


                break;
            case LIGHT_NONE: default: break;
            }
            m_LightMng->SetShadowMapName(temp.c_str(), i);
        }
    }

    CreatePSO();
}

void Scene::CheckCollsion()
{
    static bool first = true;
    if (first)
    {
        first = false;
        return;
    }
    for (int i = 0; i < m_vecObject.size(); i++)
    {
        if (!m_vecObject[i]->m_bEnable) continue;

        for (int j = i + 1; j < m_vecObject.size(); j++) m_vecObject[i]->CheckCollision(m_vecObject[j]);
    }
}

void Scene::SolveConstraint()
{
    for (int i = 0; i < m_vecObject.size(); i++) if (m_vecObject[i]->m_bEnable) m_vecObject[i]->SolveConstraint();
}

void Scene::Input(UCHAR* pKeyBuffer)
{
    POINT ptCursorPos;
    GetCursorPos(&ptCursorPos);
    XMFLOAT2 xmf2MouseMovement;
    if (TEST_MOUSE_USABLE)
    {
        xmf2MouseMovement.x = static_cast<float>(ptCursorPos.x - FRAME_BUFFER_WIDTH / 2);
        xmf2MouseMovement.y = static_cast<float>(ptCursorPos.y - FRAME_BUFFER_HEIGHT / 2);
        SetCursorPos(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);
    }
    else
    {
        xmf2MouseMovement.x = 0;
        xmf2MouseMovement.y = 0;
    }

    for_each(m_vecObject.begin(), m_vecObject.end(), [&](Object* o) { o->Input(pKeyBuffer, xmf2MouseMovement); });

    if (pKeyBuffer[_M] & 0xF0)
    {
        TEST_MOUSE_USABLE = true;
        ShowCursor(false);
    }
    if (pKeyBuffer[_N] & 0xF0)
    {
        TEST_MOUSE_USABLE = false;
        ShowCursor(true);
    }

    if (1 == startEndState)
    {
        if (pKeyBuffer[_Space] & 0xF0)
        {
            startEndState     = 0;
            TEST_MOUSE_USABLE = true;
            ShowCursor(false);
        }
    }
    else if (2 == startEndState) if (pKeyBuffer[_Space] & 0xF0) startEndState = 1;
}

void Scene::Update(float fTimeElapsed)
{
    m_fCurrentTime += fTimeElapsed;
    memcpy(&m_pcbMappedPassInfo->m_xmfCurrentTime, &m_fCurrentTime, sizeof(float));

    CheckCollsion();
    SolveConstraint();

    for_each(m_vecObject.begin(), m_vecObject.end(), [&fTimeElapsed](Object* o) { if (o->m_bEnable) o->Update(fTimeElapsed); });
}

void Scene::Release()
{
    if (m_pd3dcbPassInfo)
    {
        m_pd3dcbPassInfo->Unmap(0, nullptr);
        m_pd3dcbPassInfo->Release();
    }
    
    // Cleanup GameObjects safely to avoid double deletion
    std::set<Object*> objectsToDelete;
    for (Object* obj : m_vecObject) objectsToDelete.insert(obj);
    for (Object* obj : m_vecParticlePool) objectsToDelete.insert(obj);

    for (Object* obj : objectsToDelete)
    {
        if (obj) delete obj;
    }
    m_vecObject.clear();
    m_vecParticlePool.clear();

    // Cleanup Screen Objects
    for (Screen* screen : m_vecScreenObject) delete screen;
    m_vecScreenObject.clear();

    // Cleanup LightManager
    if (m_LightMng)
    {
        delete m_LightMng;
        m_LightMng = nullptr;
    }
    
    // Cleanup DirectX Resources
    if (m_pd3dRootSignature) m_pd3dRootSignature->Release();
    if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
    if (m_pd3duabHDRAvgLum) m_pd3duabHDRAvgLum->Release();
}

void Scene::Clear()
{
    eventCount    = 0;
    startEndState = 2; // End;

    for (Object* obj : m_vecObject) delete obj;
    m_vecObject.clear();

    m_vecNonAnimObjectRenderGroup.clear();
    m_vecAnimObjectRenderGroup.clear();
    m_vecEffectRenderGroup.clear();
    m_vecUIRenderGroup.clear();
    m_vecParticleEmitter.clear();

    // Note: m_vecParticlePool objects are not in m_vecObject? 
    // Checking BuildObject(), they are separate.
    for (Object* obj : m_vecParticlePool) delete obj;
    m_vecParticlePool.clear();

    BuildObject();

    TEST_MOUSE_USABLE = false;
    ShowCursor(true);
}

void Scene::Victory() { startEndState = 3; }

void Scene::Defeat() {}

void Scene::AddObject(Object* pObject, RenderGroup renderGroup)
{
    m_vecObject.push_back(pObject);

    switch (renderGroup)
    {
    case RenderGroup::OBJECT: m_vecNonAnimObjectRenderGroup.push_back(pObject);
        break;
    case RenderGroup::ANIMATED: m_vecAnimObjectRenderGroup.push_back(pObject);
        break;
    case RenderGroup::PARTICLE: m_vecParticleEmitter.push_back(pObject);
        break;
    case RenderGroup::EFFECT: m_vecEffectRenderGroup.push_back(pObject);
        break;
    }
}

void Scene::DeleteObject(Object* pObject) {}

Object* Scene::FindObjectByName(const char* strName)
{
    for (auto iter = m_vecObject.begin(); iter != m_vecObject.end(); ++iter) if (strName == (*iter)->m_strName) return *iter;
    return nullptr;
}
