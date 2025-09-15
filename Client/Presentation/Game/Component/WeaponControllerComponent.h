#pragma once
#include "Presentation/Game/Component/Component.h"

class TransformComponent;
class CameraComponent;

class WeaponControllerComponent : public Component
{
public:
    WeaponControllerComponent() = delete;
    WeaponControllerComponent(GameObject* pObject, GameObject* pMuzzle, GameObject* pBullet);
    ~WeaponControllerComponent() override;

    void CheckCollision(Component* other) override;
    void SolveConstraint() override;
    void Update(float fTimeElapsed) override;

    void Fire();
    void Reload();

    void SetCam(GameObject* pCam);

    // For Ammo
    int   m_maxAmmo;
    int   m_curAmmo;
    bool  m_bReloading      = false;
    float m_fReloadProgress = 0;

    float m_fReloadTime = 3.0f;

protected:
    GameObject* m_pBullet;
    GameObject* m_pMuzzle;
    float   m_fCooltime;
    float   m_fCurrCooltime;

private:
    bool       m_fTryRaycast        = false;
    float      m_fMinLength         = FLT_MAX;
    Component* m_pCollided          = nullptr;
    XMFLOAT3   m_xmf3CollisionPoint = XMFLOAT3(0, 0, 0);


    TransformComponent* muzzleTransform;
    TransformComponent* myTransform;
    TransformComponent* camTransform;
    CameraComponent*    cam;
};
