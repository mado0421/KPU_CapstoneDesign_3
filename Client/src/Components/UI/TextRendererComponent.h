#pragma once
#include "../../Engine/ECS/Component.h"

//Text占쏙옙 占쏙옙占싱댐옙 80占쌘뤄옙 占쏙옙占쏙옙.

struct FontCharData
{
    int x, y, width, height, xoffset, yoffset, xadvance, page, chnl;
};

struct Font
{
    string                           name;
    unordered_map<int, FontCharData> charData;
};

namespace FontFunc
{
    vector<string> Split(string str, char delimiter);
    Font           ImportFont(const char* strFontName);
}

class LetterRenderer
{
public:
    LetterRenderer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
    ~LetterRenderer();

    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, FontCharData& charData, XMFLOAT2& screenPos, int size, int& offset);

protected:
    void CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle);
    void SetCBVGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);

private:
    // Render
    ID3D12Resource*          m_pd3dVertexBuffer;
    ID3D12Resource*          m_pd3dVertexUploadBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;

    // FontCharData
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorHandle;
    ID3D12Resource*             m_pd3dCBResource  = nullptr;
    XMFLOAT4X4*                 m_pCBFontCharData = nullptr;
};


class TextRendererComponent : public Component
{
public:
    TextRendererComponent() = delete;
    TextRendererComponent(Object* pObject, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle);
    ~TextRendererComponent() override;

    void Update(float fTimeElapsed) override;
    void Render(ID3D12GraphicsCommandList* pd3dCommandList) override;

    void Initialize(const char* strFontName);

    void SetText(const char* str);
    void SetSize(int size);
    void SetMaterialByName(const char* strMaterialName);

private:
    enum class Alignment
    {
        LEFT,
        MIDDLE,
        RIGHT
    };

    string text;

    Alignment m_alignment = Alignment::LEFT;
    Font      m_font;
    int       m_size            = 32;
    string    m_strMaterialName = "";

    TransformComponent* transform;

    vector<LetterRenderer*> m_vecLetterRenderer;
};
