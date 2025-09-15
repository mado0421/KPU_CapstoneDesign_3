#pragma once
#include "TempCharacter.h"
#include "Presentation/Game/Component/Component.h"

class TransformComponent;
class InputManagerComponent;

class HumanoidControllerComponent : public TempCharacter
{
public:
    HumanoidControllerComponent() = delete;
    HumanoidControllerComponent(GameObject* pObject, GameObject* pWeapon);
    ~HumanoidControllerComponent() override;

    void SetLookAt(GameObject* pObejct);

    void Damage(int) override;
    void Update(float fTimeElapsed) override;

    float m_fTime;

    // For Movement Part
    XMFLOAT3 m_xmf3Velocity;
    float    m_fSpeed;
    float    m_fDragFactor;

    float   m_fAimProgress;
    float   m_fTimeForAim;
    GameObject* m_pWeaponObject;

    TransformComponent*    m_pLookAt    = nullptr;
    float                  lookAtYAngle = 0;
    InputManagerComponent* l_pInput;
    TransformComponent*    l_transform;
};
