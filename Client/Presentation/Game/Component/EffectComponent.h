#pragma once
#include "Component.h"

class EffectComponent : public Component
{
public:
    EffectComponent() = delete;
    EffectComponent(GameObject* pObject);
    ~EffectComponent() override;

    void Update(float fTimeElapsed) override;

    void SetDuration(float fTime);
    void TurnOn();
    void TurnOff();

    float m_fDuration;

private:
    float m_fLifetime;
};
