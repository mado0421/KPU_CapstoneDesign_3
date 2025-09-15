#include "pch.h"

#include "Presentation/Game/GameObject.h"
#include "Presentation/Game/Component/Components.h"

EffectComponent::EffectComponent(GameObject* pObject) : Component(pObject), m_fDuration(0.0f), m_fLifetime(0.0f) { SetActive(false); }

EffectComponent::~EffectComponent() = default;

void EffectComponent::Update(float fTimeElapsed)
{
    if (!IsEnabled()) return;

    m_fLifetime -= fTimeElapsed;
    if (0 >= m_fLifetime) TurnOff();
}

void EffectComponent::SetDuration(float fTime) { m_fDuration = fTime; }

void EffectComponent::TurnOn()
{
    SetActive(true);
    MeshRendererComponent* renderer = GetGameObject()->GetComponent<MeshRendererComponent>();
    renderer->SetActive(true);
    m_fLifetime = m_fDuration;
}

void EffectComponent::TurnOff()
{
    SetActive(false);
    MeshRendererComponent* renderer = GetGameObject()->GetComponent<MeshRendererComponent>();
    renderer->SetActive(false);
}
