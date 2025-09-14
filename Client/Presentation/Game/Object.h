#pragma once
#include "Component/Component.h"

struct CB_OBJECT_INFO
{
    XMFLOAT4X4 xmf4x4World;
};

class Mesh;
class MeshData;

class Object
{
public:
    Object();
    Object(const char*);
    virtual ~Object();

    virtual void CheckCollision(Object* other);
    virtual void SolveConstraint();
    virtual void Input(UCHAR* pKeyBuffer, XMFLOAT2& xmf2MouseMovement);
    virtual void Update(float fTimeElapsed);
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

    virtual void SetParent(Object* pObject) { m_pParent = pObject; }
    void         SetActive(bool state);

    template <typename T>
    T* FindComponent();

    template <typename T>
    vector<T*> FindComponents();

    template <typename T>
    vector<T*> FindComponentsInChildren();

    void AddComponent(Component* component);

protected:
    template <typename T>
    void FindComponentsReq(vector<T*>& result);

public:
    string          m_strName;
    bool            m_bEnable;
    float           m_fTime;
    Object*         m_pParent;
    vector<Object*> m_vecpChild;

    vector<Component*> m_vecComponents;
};

class Screen
{
public:
    Screen() = delete;
    Screen(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, D3D12_CPU_DESCRIPTOR_HANDLE& d3dCbvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE& d3dCbvGPUDescriptorStartHandle, float width, float height);
    ~Screen();

    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

private:
    Mesh*                       m_pScreenMesh        = nullptr;
    ID3D12Resource*             m_pd3dCBResource     = nullptr;
    XMFLOAT4X4*                 m_pCBMappedTransform = nullptr;
    D3D12_GPU_DESCRIPTOR_HANDLE m_d3dCbvGPUDescriptorHandle;
};

template <typename T>
T* Object::FindComponent()
{
    for (Component* c : m_vecComponents)
    {
        T* as = dynamic_cast<T*>(c);
        if (nullptr != as) return as;
    }
    return nullptr;
}

template <typename T>
vector<T*> Object::FindComponents()
{
    vector<T*> result;

    for (Component* c : m_vecComponents)
    {
        T* as = dynamic_cast<T*>(c);
        if (nullptr != as) result.push_back(as);
    }

    return result;
}

template <typename T>
vector<T*> Object::FindComponentsInChildren()
{
    vector<T*> result;

    FindComponentsReq(result);

    return result;
}

template <typename T>
void Object::FindComponentsReq(vector<T*>& result)
{
    for (Component* c : m_vecComponents)
    {
        T* as = dynamic_cast<T*>(c);
        if (nullptr != as) result.push_back(as);
    }

    for (Object* c : m_vecpChild) c->FindComponentsReq<T>(result);
}
