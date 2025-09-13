#pragma once
#include "d3dx12.h"

class DirectXMethods
{
public:
	
};


inline ID3D12Resource* CreateBufferResource(ID3D12Device*              pd3dDevice,
                                            ID3D12GraphicsCommandList* pd3dCommandList,
                                            void*                      pData,
                                            UINT                       nBytes,
                                            D3D12_HEAP_TYPE            d3dHeapType       = D3D12_HEAP_TYPE_UPLOAD,
                                            D3D12_RESOURCE_STATES      d3dResourceStates =
                                                    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                                            ID3D12Resource** ppd3dUploadBuffer = nullptr)
{
    ID3D12Resource* pd3dBuffer = nullptr;

    D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
    ::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
    d3dHeapPropertiesDesc.Type                 = d3dHeapType;
    d3dHeapPropertiesDesc.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    d3dHeapPropertiesDesc.CreationNodeMask     = 1;
    d3dHeapPropertiesDesc.VisibleNodeMask      = 1;

    D3D12_RESOURCE_DESC d3dResourceDesc;
    ::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
    d3dResourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
    d3dResourceDesc.Alignment          = 0;
    d3dResourceDesc.Width              = nBytes;
    d3dResourceDesc.Height             = 1;
    d3dResourceDesc.DepthOrArraySize   = 1;
    d3dResourceDesc.MipLevels          = 1;
    d3dResourceDesc.Format             = DXGI_FORMAT_UNKNOWN;
    d3dResourceDesc.SampleDesc.Count   = 1;
    d3dResourceDesc.SampleDesc.Quality = 0;
    d3dResourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    d3dResourceDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

    D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
    if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
    else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

    HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE,
                                                          &d3dResourceDesc, d3dResourceInitialStates, nullptr,
                                                          __uuidof(ID3D12Resource), (void**)&pd3dBuffer);

    if (pData)
    {
        switch (d3dHeapType)
        {
        case D3D12_HEAP_TYPE_DEFAULT:
            {
                if (ppd3dUploadBuffer)
                {
                    d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
                    pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc,
                                                        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                        __uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);
#ifdef _WITH_MAPPING
                    D3D12_RANGE d3dReadRange = {0, 0}; UINT8* pBufferDataBegin = NULL; (*ppd3dUploadBuffer)->
                            Map(0, &d3dReadRange, (void**)&pBufferDataBegin); memcpy(pBufferDataBegin, pData, nBytes); (
                        *ppd3dUploadBuffer)->Unmap(0, NULL); pd3dCommandList->CopyResource(
                        pd3dBuffer, *ppd3dUploadBuffer);
#else
                    D3D12_SUBRESOURCE_DATA d3dSubResourceData;
                    ::ZeroMemory(&d3dSubResourceData, sizeof(D3D12_SUBRESOURCE_DATA));
                    d3dSubResourceData.pData      = pData;
                    d3dSubResourceData.SlicePitch = d3dSubResourceData.RowPitch = nBytes;
                    ::UpdateSubresources<1>(pd3dCommandList, pd3dBuffer, *ppd3dUploadBuffer, 0, 0, 1,
                                            &d3dSubResourceData);

#endif
                    D3D12_RESOURCE_BARRIER d3dResourceBarrier;
                    ::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
                    d3dResourceBarrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                    d3dResourceBarrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                    d3dResourceBarrier.Transition.pResource   = pd3dBuffer;
                    d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
                    d3dResourceBarrier.Transition.StateAfter  = d3dResourceStates;
                    d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                    pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
                }
                break;
            }
        case D3D12_HEAP_TYPE_UPLOAD:
            {
                D3D12_RANGE d3dReadRange     = {0, 0};
                UINT8*      pBufferDataBegin = nullptr;
                pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
                memcpy(pBufferDataBegin, pData, nBytes);
                pd3dBuffer->Unmap(0, nullptr);
                break;
            }
        case D3D12_HEAP_TYPE_READBACK: break;
        }
    }
    return pd3dBuffer;
}

inline ID3D12Resource* CreateTextureResourceFromFile(ID3D12Device*              pd3dDevice,
                                                     ID3D12GraphicsCommandList* pd3dCommandList,
                                                     const wchar_t*             pszFileName,
                                                     ID3D12Resource**           ppd3dUploadBuffer,
                                                     D3D12_RESOURCE_STATES      d3dResourceStates =
                                                             D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
{
    ID3D12Resource*                     pd3dTexture = nullptr;
    std::unique_ptr<uint8_t[]>          ddsData;
    std::vector<D3D12_SUBRESOURCE_DATA> vSubresources;
    DDS_ALPHA_MODE                      ddsAlphaMode = DDS_ALPHA_MODE_UNKNOWN;
    bool                                bIsCubeMap   = false;

    HRESULT hResult = LoadDDSTextureFromFileEx(pd3dDevice, pszFileName, 0, D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT,
                                               &pd3dTexture, ddsData, vSubresources, &ddsAlphaMode, &bIsCubeMap);

    D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
    ::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
    d3dHeapPropertiesDesc.Type                 = D3D12_HEAP_TYPE_UPLOAD;
    d3dHeapPropertiesDesc.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    d3dHeapPropertiesDesc.CreationNodeMask     = 1;
    d3dHeapPropertiesDesc.VisibleNodeMask      = 1;

    assert(pd3dTexture && "ERROR CreateTextureResourceFromFile");
    D3D12_RESOURCE_DESC d3dTextureResourceDesc = pd3dTexture->GetDesc();
    UINT                nSubResources          = static_cast<UINT>(vSubresources.size());
    UINT64              nBytes                 = GetRequiredIntermediateSize(pd3dTexture, 0, nSubResources);
    //	UINT nSubResources = d3dTextureResourceDesc.DepthOrArraySize * d3dTextureResourceDesc.MipLevels;
    //	UINT64 nBytes = 0;
    //	pd3dDevice->GetCopyableFootprints(&d3dTextureResourceDesc, 0, nSubResources, 0, NULL, NULL, NULL, &nBytes);

    D3D12_RESOURCE_DESC d3dBufferResourceDesc;
    ::ZeroMemory(&d3dBufferResourceDesc, sizeof(D3D12_RESOURCE_DESC));
    d3dBufferResourceDesc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER; //Upload Heap���� �ؽ��ĸ� ������ �� ����
    d3dBufferResourceDesc.Alignment          = 0;
    d3dBufferResourceDesc.Width              = nBytes;
    d3dBufferResourceDesc.Height             = 1;
    d3dBufferResourceDesc.DepthOrArraySize   = 1;
    d3dBufferResourceDesc.MipLevels          = 1;
    d3dBufferResourceDesc.Format             = DXGI_FORMAT_UNKNOWN;
    d3dBufferResourceDesc.SampleDesc.Count   = 1;
    d3dBufferResourceDesc.SampleDesc.Quality = 0;
    d3dBufferResourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    d3dBufferResourceDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

    hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dBufferResourceDesc,
                                                  D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource),
                                                  (void**)ppd3dUploadBuffer);

    //UINT nSubResources = (UINT)vSubresources.size();
    //D3D12_SUBRESOURCE_DATA *pd3dSubResourceData = new D3D12_SUBRESOURCE_DATA[nSubResources];
    //for (UINT i = 0; i < nSubResources; i++) pd3dSubResourceData[i] = vSubresources.at(i);

    //	std::vector<D3D12_SUBRESOURCE_DATA>::pointer ptr = &vSubresources[0];
    UINT64 nBytesUpdated = UpdateSubresources(pd3dCommandList, pd3dTexture, *ppd3dUploadBuffer, 0, 0, nSubResources,
                                              &vSubresources[0]);

    D3D12_RESOURCE_BARRIER d3dResourceBarrier;
    ::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
    d3dResourceBarrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    d3dResourceBarrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    d3dResourceBarrier.Transition.pResource   = pd3dTexture;
    d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    d3dResourceBarrier.Transition.StateAfter  = d3dResourceStates;
    d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

    //	delete[] pd3dSubResourceData;

    return pd3dTexture;
}

inline D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
    d3dShaderResourceViewDesc.Format                  = d3dResourceDesc.Format;
    d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    switch (nTextureType)
    {
    case RESOURCE_TEXTURE2D:
    //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
    case RESOURCE_TEXTURE2D_ARRAY: d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        d3dShaderResourceViewDesc.Texture2D.MipLevels           = -1;
        d3dShaderResourceViewDesc.Texture2D.MostDetailedMip     = 0;
        d3dShaderResourceViewDesc.Texture2D.PlaneSlice          = 0;
        d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
        break;
    case RESOURCE_TEXTURE2DARRAY:
        //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
        d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        d3dShaderResourceViewDesc.Texture2DArray.MipLevels           = -1;
        d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip     = 0;
        d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice          = 0;
        d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
        d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice     = 0;
        d3dShaderResourceViewDesc.Texture2DArray.ArraySize           = d3dResourceDesc.DepthOrArraySize;
        break;
    case RESOURCE_TEXTURE_CUBE:
        //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
        d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        d3dShaderResourceViewDesc.TextureCube.MipLevels           = -1;
        d3dShaderResourceViewDesc.TextureCube.MostDetailedMip     = 0;
        d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
        break;
    case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
        d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        d3dShaderResourceViewDesc.Buffer.FirstElement        = 0;
        d3dShaderResourceViewDesc.Buffer.NumElements         = 0;
        d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
        d3dShaderResourceViewDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
        break;
    }
    return d3dShaderResourceViewDesc;
}