#include "pch.h"
#include "Object.h"
#include "Presentation/Game/Component/Components.h"

Object::Object() : m_strName(""), m_bEnable(true), m_fTime(0.0f), m_pParent(nullptr) {}

Object::Object(const char* strName) : m_strName(strName), m_bEnable(true), m_fTime(0.0f), m_pParent(nullptr) {}

Object::~Object() { for_each(m_vecComponents.begin(), m_vecComponents.end(), [](Component* c) { delete c; }); }

void Object::CheckCollision(Object* other)
{
    for_each(m_vecComponents.begin(), m_vecComponents.end(), [&](Component* c)
    {
        vector<ColliderComponent*> colliders = other->FindComponents<ColliderComponent>();
        for_each(colliders.begin(), colliders.end(), [&](ColliderComponent* collider) { c->CheckCollision(collider); });
    });
}

void Object::SolveConstraint() { for_each(m_vecComponents.begin(), m_vecComponents.end(), [](Component* c) { c->SolveConstraint(); }); }

void Object::Input(UCHAR* pKeyBuffer, XMFLOAT2& xmf2MouseMovement)
{
    InputManagerComponent* l_pInputMng = FindComponent<InputManagerComponent>();
    if (nullptr != l_pInputMng) l_pInputMng->InputEvent(pKeyBuffer, xmf2MouseMovement);
}

void Object::Update(float fTimeElapsed)
{
    m_fTime += fTimeElapsed;
    for_each(m_vecComponents.begin(), m_vecComponents.end(), [&](Component* c) { c->Update(fTimeElapsed); });
}

void Object::Render(ID3D12GraphicsCommandList* pd3dCommandList) { for_each(m_vecComponents.begin(), m_vecComponents.end(), [&](Component* c) { c->Render(pd3dCommandList); }); }

void Object::SetActive(bool state)
{
    m_bEnable = state;
    for_each(m_vecComponents.begin(), m_vecComponents.end(), [&](Component* c) { c->SetActive(state); });
}


void Object::AddComponent(Component* component) { m_vecComponents.push_back(component); }
