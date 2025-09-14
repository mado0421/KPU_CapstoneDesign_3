#include "pch.h"
#include "Presentation/Game/Component/Components.h"
#include "Presentation/Game/Object.h"

EffectComponent::EffectComponent(Object* pObject) : Component(pObject), m_fDuration(0.0f), m_fLifetime(0.0f) { SetActive(false); }

EffectComponent::~EffectComponent() {}

void EffectComponent::Update(float fTimeElapsed)
{
    if (!is_enable) return;

    m_fLifetime -= fTimeElapsed;
    if (0 >= m_fLifetime) TurnOff();
}

void EffectComponent::SetDuration(float fTime) { m_fDuration = fTime; }

void EffectComponent::TurnOn()
{
    SetActive(true);
    MeshRendererComponent* renderer = object->FindComponent<MeshRendererComponent>();
    renderer->SetActive(true);
    m_fLifetime = m_fDuration;
}

void EffectComponent::TurnOff()
{
    SetActive(false);
    MeshRendererComponent* renderer = object->FindComponent<MeshRendererComponent>();
    renderer->SetActive(false);
}
