#pragma once
#include "../../Character/Character.h"
#include "../../Engine/ECS/Component.h"

class Object;
class ParticleEmitterComponent;

class TargetBoardControllerComponent : public Component, public Character
{
public:
    TargetBoardControllerComponent() = delete;
    TargetBoardControllerComponent(Object* pObject, bool bAutoRevive = false);
    ~TargetBoardControllerComponent() override;

    void Update(float fTimeElapsed) override;
    void Revive() override;
    void Die() override;

    void SetPlayer(Object* pO);

private:
    float m_fTime         = 0.0f;
    float m_fAttackPeriod = 5.0f;

    Character*                m_pPlayerCharacter = nullptr;
    Object*                   pe                 = nullptr;
    ParticleEmitterComponent* lpec               = nullptr;
};
