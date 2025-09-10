#pragma once
#include "../../Character/Character.h"
#include "../../Core/Component.h"

class TransformComponent;
class InputManagerComponent;

class HumanoidControllerComponent : public Component, public Character
{
public:
    HumanoidControllerComponent() = delete;
    HumanoidControllerComponent(Object* pObject, Object* pWeapon);
    ~HumanoidControllerComponent() override;

    void SetLookAt(Object* pObejct);

    void Damage(int) override;
    void Update(float fTimeElapsed) override;

    float m_fTime;

    // For Movement Part
    XMFLOAT3 m_xmf3Velocity;
    float    m_fSpeed;
    float    m_fDragFactor;

    float   m_fAimProgress;
    float   m_fTimeForAim;
    Object* m_pWeaponObject;

    TransformComponent*    m_pLookAt    = nullptr;
    float                  lookAtYAngle = 0;
    InputManagerComponent* l_pInput;
    TransformComponent*    l_transform;
};
