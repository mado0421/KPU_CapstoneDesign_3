#include "pch.h"
#include "Scene.h"
#include "Object.h"
#include "Components.h"
#include "PipelineStateObject.h"
#include "../Graphics/Light.h"
#include "../Renderer/Elements/Texture.h"

#include "src/Renderer/DirectX/DirectXMethods.h"
#include "src/Renderer/DirectX/d3dx12.h"

void Scene::Render(D3D12_CPU_DESCRIPTOR_HANDLE hBckBufRtv, D3D12_CPU_DESCRIPTOR_HANDLE hBckBufDsv)
{
    m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);
    m_pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);
    m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    CameraComponent* cam = m_pCameraObject->FindComponent<CameraComponent>();
    cam->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
    cam->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    cam->SetViewportsAndScissorRects(m_pd3dCommandList);
    UpdatePassInfoAboutCamera();

    D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPassInfo->GetGPUVirtualAddress();
    m_pd3dCommandList->SetGraphicsRootConstantBufferView(ROOTSIGNATURE_PASSCONSTANTS, d3dGpuVirtualAddress);

    XMFLOAT4X4 texture = {0.5f, 0, 0, 0, 0, -0.5f, 0, 0, 0, 0, 1.0f, 0, 0.5f, 0.5f, 0, 1.0f};
    XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4TextureTransform, XMMatrixTranspose(XMLoadFloat4x4(&texture)));
    
    D3D12_RESOURCE_BARRIER d3dResourceBarrier[3];
    ::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER) * 3);
    d3dResourceBarrier[0].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    d3dResourceBarrier[0].Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Depth");
    d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
    d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    d3dResourceBarrier[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    d3dResourceBarrier[1].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    d3dResourceBarrier[1].Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Color");
    d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
    d3dResourceBarrier[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    d3dResourceBarrier[2].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    d3dResourceBarrier[2].Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    d3dResourceBarrier[2].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Normal");
    d3dResourceBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    d3dResourceBarrier[2].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
    d3dResourceBarrier[2].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_pd3dCommandList->ResourceBarrier(3, d3dResourceBarrier);

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = g_texture_manager.GetDSVCPUHandle("GBuffer_Depth");
    m_pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[2];
    rtvHandle[0] = g_texture_manager.GetRTVCPUHandle("GBuffer_Color");
    rtvHandle[1] = g_texture_manager.GetRTVCPUHandle("GBuffer_Normal");

    float pfClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    m_pd3dCommandList->ClearRenderTargetView(rtvHandle[0], pfClearColor, 0, nullptr);
    m_pd3dCommandList->ClearRenderTargetView(rtvHandle[1], pfClearColor, 0, nullptr);

    m_pd3dCommandList->OMSetRenderTargets(2, rtvHandle, FALSE, &dsvHandle);

    m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["AnimatedObject"]);
    for (auto iter = m_vecAnimObjectRenderGroup.begin(); iter != m_vecAnimObjectRenderGroup.end(); ++iter) (*iter)->Render(m_pd3dCommandList);
    m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PackGBuffer"]);
    for (auto iter = m_vecNonAnimObjectRenderGroup.begin(); iter != m_vecNonAnimObjectRenderGroup.end(); ++iter) (*iter)->Render(m_pd3dCommandList);


    d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Depth");
    d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_GENERIC_READ;

    d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Color");
    d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    d3dResourceBarrier[2].Transition.pResource   = g_texture_manager.GetTextureResource("GBuffer_Normal");
    d3dResourceBarrier[2].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    d3dResourceBarrier[2].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    m_pd3dCommandList->ResourceBarrier(3, d3dResourceBarrier);

    cam->SetViewport(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE, 0.0f, 1.0f);
    cam->SetScissorRect(0, 0, SHADOWMAPSIZE, SHADOWMAPSIZE);
    cam->SetViewportsAndScissorRects(m_pd3dCommandList);

    for (UINT i = 0; i < m_LightMng->GetNumLight(); i++)
    {
        if (m_LightMng->GetIsShadow(i))
        {
            m_LightMng->SetShaderResource(m_pd3dCommandList, i);

            D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = g_texture_manager.GetDSVCPUHandle(m_LightMng->GetShadowMapName(i).c_str());
            m_pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
            m_pd3dCommandList->OMSetRenderTargets(0, nullptr, TRUE, &dsvHandle);

            switch (m_LightMng->GetLightType(i))
            {
            case LIGHT_SPOT: m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SpotLightShadow"]);
                for (int i = 0; i < m_vecNonAnimObjectRenderGroup.size(); i++) m_vecNonAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);

                m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SpotLightShadowAnim"]);
                for (int i = 0; i < m_vecAnimObjectRenderGroup.size(); i++) m_vecAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);
                break;

            case LIGHT_POINT: m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PointLightShadow"]);
                for (int i = 0; i < m_vecNonAnimObjectRenderGroup.size(); i++) m_vecNonAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);

                m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PointLightShadowAnim"]);
                for (int i = 0; i < m_vecAnimObjectRenderGroup.size(); i++) m_vecAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);
                break;

            case LIGHT_DIRECTIONAL: m_LightMng->UpdateDirectionalLightOrthographicLH(cam->GetViewMatrix(), i);

                m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["DirectionalLightShadow"]);
                for (int i = 0; i < m_vecNonAnimObjectRenderGroup.size(); i++) m_vecNonAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);

                m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["DirectionalLightShadowAnim"]);
                for (int i = 0; i < m_vecAnimObjectRenderGroup.size(); i++) m_vecAnimObjectRenderGroup[i]->Render(m_pd3dCommandList);
                break;

            case LIGHT_NONE: default: break;
            }
        }
    }
    
    D3D12_CPU_DESCRIPTOR_HANDLE screenRtv = g_texture_manager.GetRTVCPUHandle("Screen");
    m_pd3dCommandList->ClearRenderTargetView(screenRtv, pfClearColor, 0, nullptr);
    m_pd3dCommandList->OMSetRenderTargets(1, &screenRtv, TRUE, &dsvHandle);

    cam->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
    cam->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
    cam->SetViewportsAndScissorRects(m_pd3dCommandList);
    
    m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["ColorFromGBuffer"]);
    g_texture_manager.UseForShaderResource("GBuffer_Normal", m_pd3dCommandList, ROOTSIGNATURE_NORMAL_TEXTURE);
    g_texture_manager.UseForShaderResource("GBuffer_Depth", m_pd3dCommandList, ROOTSIGNATURE_DEPTH_TEXTURE);
    g_texture_manager.UseForShaderResource("GBuffer_Color", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
    m_vecScreenObject[0]->Render(m_pd3dCommandList);
    
    m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Effect"]);
    for (auto iter = m_vecEffectRenderGroup.begin(); iter != m_vecEffectRenderGroup.end(); ++iter) (*iter)->Render(m_pd3dCommandList);
    m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
    m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Particle"]);
    for (auto iter = m_vecParticleEmitter.begin(); iter != m_vecParticleEmitter.end(); ++iter) (*iter)->Render(m_pd3dCommandList);
    m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    g_texture_manager.UseForShaderResource("GBuffer_Normal", m_pd3dCommandList, ROOTSIGNATURE_NORMAL_TEXTURE);
    g_texture_manager.UseForShaderResource("GBuffer_Depth", m_pd3dCommandList, ROOTSIGNATURE_DEPTH_TEXTURE);
    g_texture_manager.UseForShaderResource("GBuffer_Color", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
    m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["AddLight"]);

    for (UINT i = 0; i < m_LightMng->GetNumLight(); i++)
    {
        m_LightMng->SetShaderResource(m_pd3dCommandList, i);

        if (m_LightMng->GetIsShadow(i))
        {
            switch (m_LightMng->GetLightType(i))
            {
            case LIGHT_SPOT: g_texture_manager.UseForShaderResource(m_LightMng->GetShadowMapName(i).c_str(), m_pd3dCommandList, ROOTSIGNATURE_SHADOW_TEXTURE);
                break;
            case LIGHT_POINT: g_texture_manager.UseForShaderResource(m_LightMng->GetShadowMapName(i).c_str(), m_pd3dCommandList, ROOTSIGNATURE_CUBE_TEXTURE);
                break;
            case LIGHT_DIRECTIONAL: g_texture_manager.UseForShaderResource(m_LightMng->GetShadowMapName(i).c_str(), m_pd3dCommandList, ROOTSIGNATURE_SHADOWARRAY_TEXTURE);
                break;
            case LIGHT_NONE: default: break;
            }
        }

        m_vecScreenObject[0]->Render(m_pd3dCommandList);
    }

    if (gTestInt == 2)
    {
        m_pd3dCommandList->SetComputeRootSignature(m_pd3dRootSignature);

        m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["HDR_First"]);

        d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("DownScaled");
        d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("Screen");
        d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);

        g_texture_manager.UseForComputeShaderResourceSRV("Screen", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);                        // Render Result
        g_texture_manager.UseForComputeShaderResourceUAV("DownScaled", m_pd3dCommandList, ROOTSIGNATURE_POSTPROCESS_TEXTURE);              // DownScaled
        m_pd3dCommandList->SetComputeRootUnorderedAccessView(ROOTSIGNATURE_HDRLUMBUFFER, m_pd3duabHDRAvgLum->GetGPUVirtualAddress()); // AvgLum Buff

        m_pd3dCommandList->Dispatch(1, FRAME_BUFFER_HEIGHT / 4, 1);

        m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["HDR_Second"]);

        m_pd3dCommandList->Dispatch(1, 1, 1);

        m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["PP_Bloom"]);

        d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("DownScaled");
        d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Horizontal");
        d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);

        g_texture_manager.UseForComputeShaderResourceSRV("DownScaled", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
        g_texture_manager.UseForComputeShaderResourceUAV("Blur_Horizontal", m_pd3dCommandList, ROOTSIGNATURE_POSTPROCESS_TEXTURE);

        UINT numGroups = static_cast<UINT>(ceilf(FRAME_BUFFER_WIDTH / 4 / 1024.0f));
        m_pd3dCommandList->Dispatch(numGroups, FRAME_BUFFER_HEIGHT / 4, 1);

        m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Blur_Vertical"]);

        d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Horizontal");
        d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Vertical");
        d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);

        g_texture_manager.UseForComputeShaderResourceSRV("Blur_Horizontal", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
        g_texture_manager.UseForComputeShaderResourceUAV("Blur_Vertical", m_pd3dCommandList, ROOTSIGNATURE_POSTPROCESS_TEXTURE);

        numGroups = static_cast<UINT>(ceilf(FRAME_BUFFER_WIDTH / 4 / 256.0f));
        m_pd3dCommandList->Dispatch(numGroups, FRAME_BUFFER_HEIGHT / 4, 1);

        m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Blur_Horizontal"]);

        d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Vertical");
        d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        d3dResourceBarrier[1].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Horizontal");
        d3dResourceBarrier[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        d3dResourceBarrier[1].Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        m_pd3dCommandList->ResourceBarrier(2, d3dResourceBarrier);

        g_texture_manager.UseForComputeShaderResourceSRV("Blur_Vertical", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
        g_texture_manager.UseForComputeShaderResourceUAV("Blur_Horizontal", m_pd3dCommandList, ROOTSIGNATURE_POSTPROCESS_TEXTURE);

        numGroups = static_cast<UINT>(ceilf(FRAME_BUFFER_HEIGHT / 4 / 256.0f));
        m_pd3dCommandList->Dispatch(FRAME_BUFFER_WIDTH / 4, numGroups, 1);

        m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);
        m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["HDR_Last"]);

        d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("Blur_Horizontal");
        d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        m_pd3dCommandList->ResourceBarrier(1, d3dResourceBarrier);

        g_texture_manager.UseForShaderResource("Screen", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
        g_texture_manager.UseForShaderResource("Blur_Horizontal", m_pd3dCommandList, ROOTSIGNATURE_NORMAL_TEXTURE);
        m_pd3dCommandList->SetGraphicsRootUnorderedAccessView(ROOTSIGNATURE_HDRLUMBUFFER, m_pd3duabHDRAvgLum->GetGPUVirtualAddress());

        m_pd3dCommandList->ClearDepthStencilView(hBckBufDsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
        m_pd3dCommandList->OMSetRenderTargets(1, &hBckBufRtv, TRUE, &hBckBufDsv);
        m_vecScreenObject[0]->Render(m_pd3dCommandList);

        d3dResourceBarrier[0].Transition.pResource   = g_texture_manager.GetTextureResource("Screen");
        d3dResourceBarrier[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        d3dResourceBarrier[0].Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
        m_pd3dCommandList->ResourceBarrier(1, d3dResourceBarrier);

        m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

        m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["Text"]);
        for (auto iter = m_vecUIRenderGroup.begin(); iter != m_vecUIRenderGroup.end(); ++iter) (*iter)->Render(m_pd3dCommandList);
        m_pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        if (startEndState)
        {
            m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);
            m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SRToRt"]);

            if (1 == startEndState) g_texture_manager.UseForShaderResource("titleImage_rescale", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
            else if (3 == startEndState) g_texture_manager.UseForShaderResource("victory", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);
            else g_texture_manager.UseForShaderResource("endImage_rescale", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);

            m_pd3dCommandList->ClearDepthStencilView(hBckBufDsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
            m_pd3dCommandList->OMSetRenderTargets(1, &hBckBufRtv, TRUE, &hBckBufDsv);
            m_vecScreenObject[0]->Render(m_pd3dCommandList);
        }
    }
    else
    {
        m_pd3dCommandList->SetGraphicsRootSignature(m_pd3dRootSignature);
        m_pd3dCommandList->SetPipelineState(m_uomPipelineStates["SRToRt"]);

        g_texture_manager.UseForShaderResource("Screen", m_pd3dCommandList, ROOTSIGNATURE_COLOR_TEXTURE);

        m_pd3dCommandList->ClearDepthStencilView(hBckBufDsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
        m_pd3dCommandList->OMSetRenderTargets(1, &hBckBufRtv, TRUE, &hBckBufDsv);
        m_vecScreenObject[0]->Render(m_pd3dCommandList);
    }
}

ID3D12RootSignature* Scene::CreateRootSignature()
{
    ID3D12RootSignature*   pd3dGraphicsRootSignature = nullptr;
    D3D12_DESCRIPTOR_RANGE d3dDescriptorRange[10];

    d3dDescriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    d3dDescriptorRange[0].NumDescriptors                    = 1;
    d3dDescriptorRange[0].BaseShaderRegister                = ROOTSIGNATURE_OBJECTS;
    d3dDescriptorRange[0].RegisterSpace                     = 0;
    d3dDescriptorRange[0].OffsetInDescriptorsFromTableStart = 0;

    d3dDescriptorRange[1].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    d3dDescriptorRange[1].NumDescriptors                    = 1;
    d3dDescriptorRange[1].BaseShaderRegister                = ROOTSIGNATURE_LIGHTS;
    d3dDescriptorRange[1].RegisterSpace                     = 0;
    d3dDescriptorRange[1].OffsetInDescriptorsFromTableStart = 0;

    d3dDescriptorRange[2].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    d3dDescriptorRange[2].NumDescriptors                    = 1;
    d3dDescriptorRange[2].BaseShaderRegister                = ROOTSIGNATURE_COLOR_TEXTURE;
    d3dDescriptorRange[2].RegisterSpace                     = 0;
    d3dDescriptorRange[2].OffsetInDescriptorsFromTableStart = 0;

    d3dDescriptorRange[3].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    d3dDescriptorRange[3].NumDescriptors                    = 1;
    d3dDescriptorRange[3].BaseShaderRegister                = ROOTSIGNATURE_NORMAL_TEXTURE;
    d3dDescriptorRange[3].RegisterSpace                     = 0;
    d3dDescriptorRange[3].OffsetInDescriptorsFromTableStart = 0;

    d3dDescriptorRange[4].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    d3dDescriptorRange[4].NumDescriptors                    = 1;
    d3dDescriptorRange[4].BaseShaderRegister                = ROOTSIGNATURE_DEPTH_TEXTURE;
    d3dDescriptorRange[4].RegisterSpace                     = 0;
    d3dDescriptorRange[4].OffsetInDescriptorsFromTableStart = 0;

    d3dDescriptorRange[5].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    d3dDescriptorRange[5].NumDescriptors                    = 1;
    d3dDescriptorRange[5].BaseShaderRegister                = ROOTSIGNATURE_SHADOW_TEXTURE;
    d3dDescriptorRange[5].RegisterSpace                     = 0;
    d3dDescriptorRange[5].OffsetInDescriptorsFromTableStart = 0;

    d3dDescriptorRange[6].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    d3dDescriptorRange[6].NumDescriptors                    = 1;
    d3dDescriptorRange[6].BaseShaderRegister                = ROOTSIGNATURE_CUBE_TEXTURE;
    d3dDescriptorRange[6].RegisterSpace                     = 0;
    d3dDescriptorRange[6].OffsetInDescriptorsFromTableStart = 0;

    d3dDescriptorRange[7].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    d3dDescriptorRange[7].NumDescriptors                    = 1;
    d3dDescriptorRange[7].BaseShaderRegister                = ROOTSIGNATURE_SHADOWARRAY_TEXTURE;
    d3dDescriptorRange[7].RegisterSpace                     = 0;
    d3dDescriptorRange[7].OffsetInDescriptorsFromTableStart = 0;

    d3dDescriptorRange[8].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    d3dDescriptorRange[8].NumDescriptors                    = 1;
    d3dDescriptorRange[8].BaseShaderRegister                = ROOTSIGNATURE_ANIMTRANSFORM;
    d3dDescriptorRange[8].RegisterSpace                     = 0;
    d3dDescriptorRange[8].OffsetInDescriptorsFromTableStart = 0;

    d3dDescriptorRange[9].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    d3dDescriptorRange[9].NumDescriptors                    = 1;
    d3dDescriptorRange[9].BaseShaderRegister                = ROOTSIGNATURE_POSTPROCESS_TEXTURE;
    d3dDescriptorRange[9].RegisterSpace                     = 0;
    d3dDescriptorRange[9].OffsetInDescriptorsFromTableStart = 0;

    D3D12_ROOT_PARAMETER pd3dRootParameters[12];

    pd3dRootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    pd3dRootParameters[0].Descriptor.ShaderRegister = ROOTSIGNATURE_PASSCONSTANTS;
    pd3dRootParameters[0].Descriptor.RegisterSpace  = 0;
    pd3dRootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[1].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
    pd3dRootParameters[1].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[0];
    pd3dRootParameters[1].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[2].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
    pd3dRootParameters[2].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[1];
    pd3dRootParameters[2].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[3].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
    pd3dRootParameters[3].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[2];
    pd3dRootParameters[3].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[4].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
    pd3dRootParameters[4].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[3];
    pd3dRootParameters[4].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[5].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
    pd3dRootParameters[5].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[4];
    pd3dRootParameters[5].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[6].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
    pd3dRootParameters[6].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[5];
    pd3dRootParameters[6].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[7].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
    pd3dRootParameters[7].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[6];
    pd3dRootParameters[7].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[8].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
    pd3dRootParameters[8].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[7];
    pd3dRootParameters[8].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[9].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
    pd3dRootParameters[9].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[8];
    pd3dRootParameters[9].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[10].ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
    pd3dRootParameters[10].DescriptorTable.pDescriptorRanges   = &d3dDescriptorRange[9];
    pd3dRootParameters[10].ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;

    pd3dRootParameters[11].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
    pd3dRootParameters[11].Descriptor.ShaderRegister = ROOTSIGNATURE_HDRLUMBUFFER;
    pd3dRootParameters[11].Descriptor.RegisterSpace  = 0;
    pd3dRootParameters[11].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc[2];
    ::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC) * 2);
    d3dSamplerDesc[0].Filter           = D3D12_FILTER_ANISOTROPIC;
    d3dSamplerDesc[0].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    d3dSamplerDesc[0].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    d3dSamplerDesc[0].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    d3dSamplerDesc[0].MipLODBias       = 0;
    d3dSamplerDesc[0].MaxAnisotropy    = 1;
    d3dSamplerDesc[0].ComparisonFunc   = D3D12_COMPARISON_FUNC_ALWAYS;
    d3dSamplerDesc[0].MinLOD           = 0;
    d3dSamplerDesc[0].MaxLOD           = D3D12_FLOAT32_MAX;
    d3dSamplerDesc[0].ShaderRegister   = 0;
    d3dSamplerDesc[0].RegisterSpace    = 0;
    d3dSamplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    d3dSamplerDesc[1].Filter           = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    d3dSamplerDesc[1].AddressU         = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    d3dSamplerDesc[1].AddressV         = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    d3dSamplerDesc[1].AddressW         = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    d3dSamplerDesc[1].BorderColor      = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
    d3dSamplerDesc[1].MipLODBias       = 0;
    d3dSamplerDesc[1].MaxAnisotropy    = 1;
    d3dSamplerDesc[1].ComparisonFunc   = D3D12_COMPARISON_FUNC_LESS;
    d3dSamplerDesc[1].MinLOD           = 0;
    d3dSamplerDesc[1].MaxLOD           = D3D12_FLOAT32_MAX;
    d3dSamplerDesc[1].ShaderRegister   = 1;
    d3dSamplerDesc[1].RegisterSpace    = 0;
    d3dSamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
    D3D12_ROOT_SIGNATURE_DESC  d3dRootSignatureDesc;
    ::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
    d3dRootSignatureDesc.NumParameters     = _countof(pd3dRootParameters);
    d3dRootSignatureDesc.pParameters       = pd3dRootParameters;
    d3dRootSignatureDesc.NumStaticSamplers = 2;
    d3dRootSignatureDesc.pStaticSamplers   = &d3dSamplerDesc[0];
    d3dRootSignatureDesc.Flags             = d3dRootSignatureFlags;

    ID3DBlob* pd3dSignatureBlob = nullptr;
    ID3DBlob* pd3dErrorBlob     = nullptr;
    HRESULT   isSuccess         = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
    isSuccess                   = m_pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
    if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
    if (pd3dErrorBlob) pd3dErrorBlob->Release();

    return pd3dGraphicsRootSignature;
}

void Scene::CreateDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
    d3dDescriptorHeapDesc.NumDescriptors = MAXNUMCBV + MAXNUMSRV;
    d3dDescriptorHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    d3dDescriptorHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    d3dDescriptorHeapDesc.NodeMask       = 0;

    HRESULT result = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dCbvSrvDescriptorHeap);
    HRESULT reason = m_pd3dDevice->GetDeviceRemovedReason();

    m_d3dCbvCPUDescriptorStartHandle     = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    m_d3dCbvGPUDescriptorStartHandle     = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
    m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + gnCbvSrvDescriptorIncrementSize * MAXNUMCBV;
    m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + gnCbvSrvDescriptorIncrementSize * MAXNUMCBV;
}

void Scene::CreatePSO()
{
    auto PackGBufferPso                = PackGBufferPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["PackGBuffer"] = PackGBufferPso.GetPipelineState();

    auto AnimatedObjectPso                = AnimatedObjectPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["AnimatedObject"] = AnimatedObjectPso.GetPipelineState();

    auto RenderShadowPso                        = RenderShadowPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["SpotLightShadow"]      = RenderShadowPso.GetPipelineState();
    auto RenderSpotLightShadowAnimatedObjectPso = RenderSpotLightShadowAnimatedObjectPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["SpotLightShadowAnim"]  = RenderSpotLightShadowAnimatedObjectPso.GetPipelineState();

    auto RenderPointLightShadowPso               = RenderPointLightShadowPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["PointLightShadow"]      = RenderPointLightShadowPso.GetPipelineState();
    auto RenderPointLightShadowAnimatedObjectPso = RenderPointLightShadowAnimatedObjectPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["PointLightShadowAnim"]  = RenderPointLightShadowAnimatedObjectPso.GetPipelineState();

    auto RenderDirectionalShadowPso                   = RenderDirectionalShadowPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["DirectionalLightShadow"]     = RenderDirectionalShadowPso.GetPipelineState();
    auto RenderDirectionalShadowAnimatedObjectPso     = RenderDirectionalShadowAnimatedObjectPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["DirectionalLightShadowAnim"] = RenderDirectionalShadowAnimatedObjectPso.GetPipelineState();

    auto ColorFromGBufferPso                = ColorFromGBufferPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["ColorFromGBuffer"] = ColorFromGBufferPso.GetPipelineState();

    auto AddLightPso                = AddLightPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["AddLight"] = AddLightPso.GetPipelineState();

    auto EffectPso                = EffectPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["Effect"] = EffectPso.GetPipelineState();

    auto ParticlePso                = ParticlePSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["Particle"] = ParticlePso.GetPipelineState();

    auto TextPso                = TextPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["Text"] = TextPso.GetPipelineState();

    auto VerticalBlurPso                   = VerticalBlurCPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["Blur_Vertical"]   = VerticalBlurPso.GetPipelineState();
    auto HorizontalBlurPso                 = HorizontalBlurCPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["Blur_Horizontal"] = HorizontalBlurPso.GetPipelineState();
    auto SRToRtPso                         = SRToRtPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["SRToRt"]          = SRToRtPso.GetPipelineState();

    auto HDRFstPassCpso               = HDRFstPassCPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["HDR_First"]  = HDRFstPassCpso.GetPipelineState();
    auto HDRScdPassCpso               = HDRScdPassCPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["HDR_Second"] = HDRScdPassCpso.GetPipelineState();
    auto HDRToneMappingPso            = HDRToneMappingPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["HDR_Last"]   = HDRToneMappingPso.GetPipelineState();

    auto BloomCpso                  = BloomCPSO(m_pd3dDevice, m_pd3dRootSignature);
    m_uomPipelineStates["PP_Bloom"] = BloomCpso.GetPipelineState();
}

void Scene::CreatePassInfoShaderResource()
{
    UINT ncbElementBytes = sizeof(ConstantBufferPassInfo) + 255 & ~255;
    m_pd3dcbPassInfo     = CreateBufferResource(m_pd3dDevice, m_pd3dCommandList, nullptr, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

    D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;

    if (nullptr != m_pd3dcbPassInfo)
    {
        m_pd3dcbPassInfo->Map(0, nullptr, (void**)&m_pcbMappedPassInfo);
        D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPassInfo->GetGPUVirtualAddress();
        d3dCBVDesc.SizeInBytes                         = ncbElementBytes;
        d3dCBVDesc.BufferLocation                      = d3dGpuVirtualAddress;
        m_pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorStartHandle);

        m_d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
    }

    m_d3dCbvGPUPassInfoHandle = m_d3dCbvGPUDescriptorStartHandle;
    m_d3dCbvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}

void Scene::UpdatePassInfoAboutCamera()
{
    CameraComponent* cam = m_pCameraObject->FindComponent<CameraComponent>();
    XMFLOAT4X4       xmf4x4Temp;

    xmf4x4Temp = cam->GetViewMatrix();
    XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraView, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));
    xmf4x4Temp = Matrix4x4::Inverse(xmf4x4Temp);
    XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraViewInv, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

    xmf4x4Temp = cam->GetProjectionMatrix();
    XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraProjection, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));
    xmf4x4Temp = Matrix4x4::Inverse(xmf4x4Temp);
    XMStoreFloat4x4(&m_pcbMappedPassInfo->m_xmf4x4CameraProjectionInv, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Temp)));

    XMFLOAT3 xmf3WorldPos = m_pCameraObject->FindComponent<TransformComponent>()->GetPosition(Space::world);
    memcpy(&m_pcbMappedPassInfo->m_xmf3CameraPosition, &xmf3WorldPos, sizeof(XMFLOAT3));
}