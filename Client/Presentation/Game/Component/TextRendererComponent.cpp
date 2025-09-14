#include "pch.h"
#include "Presentation/Game/Component/Components.h"

#include "Presentation/Game/Manager/MaterialManager.h"
#include "Presentation/Renderer/DirectX/DirectXMethods.h"
#include "Presentation/Renderer/Elements/Vertex.h"

vector<string> FontFunc::Split(string str, char delimiter)
{
    vector<string> answer;
    stringstream   ss(str);
    string         temp;

    while (getline(ss, temp, delimiter)) answer.push_back(temp);

    return answer;
}

Font FontFunc::ImportFont(const char* strFontName)
{
    Font   result;
    string fontAddress = strFontName;
    fontAddress        = "Resources/Fonts/font_" + fontAddress + ".fnt";

    //cout << "Font Address: " << fontAddress << "\n\n";

    ifstream     font(fontAddress);
    stringstream ss;

    ss << font.rdbuf();
    //cout << ss.str();

    vector<string> lines = Split(ss.str(), '\n');
    for (int i = 0; i < lines.size(); i++)
    {
        FontCharData temp;
        char         str[256];
        strcpy_s(str, sizeof(str), lines[i].c_str());
        char *pch, *context = nullptr;
        pch                 = strtok_s(str, " ", &context);
        int charId          = atoi(pch);
        pch                 = strtok_s(nullptr, " ", &context);
        temp.x              = atoi(pch);
        pch                 = strtok_s(nullptr, " ", &context);
        temp.y              = atoi(pch);
        pch                 = strtok_s(nullptr, " ", &context);
        temp.width          = atoi(pch);
        pch                 = strtok_s(nullptr, " ", &context);
        temp.height         = atoi(pch);
        pch                 = strtok_s(nullptr, " ", &context);
        temp.xoffset        = atoi(pch);
        pch                 = strtok_s(nullptr, " ", &context);
        temp.yoffset        = atoi(pch);
        pch                 = strtok_s(nullptr, " ", &context);
        temp.xadvance       = atoi(pch);
        pch                 = strtok_s(nullptr, " ", &context);
        temp.page           = atoi(pch);
        pch                 = strtok_s(nullptr, " ", &context);
        temp.chnl           = atoi(pch);

        result.charData[charId] = temp;
    }
    return result;
}

LetterRenderer::LetterRenderer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
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

LetterRenderer::~LetterRenderer()
{
    if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
    if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
}

void LetterRenderer::Render(ID3D12GraphicsCommandList* pd3dCommandList, FontCharData& charData, XMFLOAT2& screenPos, int size, int& offset)
{
    pd3dCommandList->SetGraphicsRootDescriptorTable(ROOTSIGNATURE_OBJECTS, m_d3dCbvGPUDescriptorHandle);
    UINT ncbElementBytes = sizeof(XMFLOAT4X4) + 255 & ~255;
    memset(m_pCBFontCharData, NULL, ncbElementBytes);

    float scale = size / 32.0;

    m_pCBFontCharData->_11 = charData.x / 256.0;
    m_pCBFontCharData->_12 = charData.y / 256.0;
    m_pCBFontCharData->_13 = charData.width / 256.0;
    m_pCBFontCharData->_14 = charData.height / 256.0;
    m_pCBFontCharData->_21 = charData.xoffset;
    m_pCBFontCharData->_22 = charData.yoffset;
    m_pCBFontCharData->_23 = charData.xadvance;
    m_pCBFontCharData->_24 = charData.page;
    m_pCBFontCharData->_31 = charData.chnl;
    m_pCBFontCharData->_32 = (screenPos.x + offset) / static_cast<float>(FRAME_BUFFER_WIDTH / 2) - 1;
    m_pCBFontCharData->_33 = (screenPos.y - charData.yoffset * 3.7 * scale) / static_cast<float>(FRAME_BUFFER_HEIGHT / 2) - 1;
    m_pCBFontCharData->_34 = static_cast<float>(size);

    pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferView);
    pd3dCommandList->DrawInstanced(1, 1, 0, 0);

    offset += charData.xadvance * 3.7 * scale;
}

void LetterRenderer::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle)
{
    D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
    UINT                            ncbElementBytes = sizeof(XMFLOAT4X4) + 255 & ~255;

    if (nullptr != m_pd3dCBResource)
    {
        m_pd3dCBResource->Map(0, nullptr, (void**)&m_pCBFontCharData);
        D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dCBResource->GetGPUVirtualAddress();
        d3dCBVDesc.SizeInBytes                         = ncbElementBytes;
        d3dCBVDesc.BufferLocation                      = d3dGpuVirtualAddress;
        pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorStartHandle);

        d3dCbvCPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
    }
}

void LetterRenderer::SetCBVGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle)
{
    m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorStartHandle;
    d3dCbvGPUDescriptorStartHandle.ptr += gnCbvSrvDescriptorIncrementSize;
}

TextRendererComponent::TextRendererComponent(Object*                      pObject,
                                             ID3D12Device*                pd3dDevice,
                                             ID3D12GraphicsCommandList*   pd3dCommandList,
                                             D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle,
                                             D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle) : Component(pObject)
{
    for (int i = 0; i < 80; i++)
    {
        auto lr = new LetterRenderer(pd3dDevice, pd3dCommandList, d3dCbvCPUDescriptorStartHandle, d3dCbvGPUDescriptorStartHandle);
        m_vecLetterRenderer.push_back(lr);
    }
}

TextRendererComponent::~TextRendererComponent() {}

void TextRendererComponent::Update(float fTimeElapsed) {}

void TextRendererComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
    g_MaterialMng.SetMaterial(m_strMaterialName.c_str(), pd3dCommandList);

    TransformComponent* transform = object->GetComponent<TransformComponent>();
    XMFLOAT2            screenPos;
    screenPos.x = transform->GetPosition().x;
    screenPos.y = transform->GetPosition().y;

    int offset = 0;

    for (int i = 0; i < text.length(); i++) m_vecLetterRenderer[i]->Render(pd3dCommandList, m_font.charData[text[i]], screenPos, m_size, offset);
}

void TextRendererComponent::Initialize(const char* strFontName) { m_font = FontFunc::ImportFont(strFontName); }

void TextRendererComponent::SetText(const char* str)
{
    text = str;
    //if(text.length() > 255)
}

void TextRendererComponent::SetSize(int size)
{
    m_size = size >= 1
                 ? size
                 : 1;
}

void TextRendererComponent::SetMaterialByName(const char* strMaterialName) { m_strMaterialName = strMaterialName; }
