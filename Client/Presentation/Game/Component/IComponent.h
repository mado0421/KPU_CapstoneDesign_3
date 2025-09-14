#pragma once

class Object;

struct IComponent
{
    IComponent() = default;
    virtual void Update(float fElapsedTime) = 0;

    Object* ownObject;
    string  name;
};

struct ITransform : IComponent
{
    ITransform();

    void       Move(XMFLOAT3 value);
    void       Rotate(XMFLOAT3 degree);
    XMFLOAT3   GetPosition() { return XMFLOAT3(m_xmf4x4world._41, m_xmf4x4world._42, m_xmf4x4world._43); }
    XMFLOAT4X4 GetTransform() { return m_xmf4x4world; }

private:
    XMFLOAT4X4 m_xmf4x4world;
};

struct IMeshRenderer : IComponent
{
    void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};

struct IMesh : IComponent {};
