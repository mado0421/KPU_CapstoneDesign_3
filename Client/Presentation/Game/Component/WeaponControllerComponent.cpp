#include "pch.h"
#include "WeaponControllerComponent.h"
#include "Presentation/Game/Component/Components.h"
#include "Presentation/Game/Object.h"
#include "Presentation/Game/Core/Scene.h"

WeaponControllerComponent::WeaponControllerComponent(Object* pObject, Object* pMuzzle, Object* pBullet) : Component(pObject),
                                                                                                          m_maxAmmo(10),
                                                                                                          m_curAmmo(m_maxAmmo),
                                                                                                          m_bReloading(false),
                                                                                                          m_fReloadProgress(0),
                                                                                                          m_pBullet(pBullet),
                                                                                                          m_pMuzzle(pMuzzle),
                                                                                                          m_fCooltime(0.5f),
                                                                                                          m_fCurrCooltime(0.0f),
                                                                                                          camTransform(nullptr),
                                                                                                          cam(nullptr)
{
    myTransform     = object->FindComponent<TransformComponent>();
    muzzleTransform = m_pMuzzle->FindComponent<TransformComponent>();
}

WeaponControllerComponent::~WeaponControllerComponent() {}

void WeaponControllerComponent::CheckCollision(Component* other)
{
    if (!is_enable || !other->is_enable) return;

    if (m_fTryRaycast)
    {
        TransformComponent* muzzle = muzzleTransform;
        XMFLOAT3            xmf3Origin, xmf3Direction;
        XMVECTOR            origin,     direction;
        float               length = 0;


        if (cam)
        {
            xmf3Origin    = camTransform->GetPosition(Space::world);
            xmf3Direction = cam->GetLookVector();

            origin    = XMLoadFloat3(&xmf3Origin);
            direction = XMLoadFloat3(&xmf3Direction);

            auto otherBoxCollider = dynamic_cast<BoxColliderComponent*>(other);
            if (otherBoxCollider)
            {
                otherBoxCollider->m_box.Intersects(origin, direction, length);
                if (0 < length)
                {
                    if (m_fMinLength > length)
                    {
                        m_fMinLength = length;
                        m_pCollided  = otherBoxCollider;
                        XMStoreFloat3(&m_xmf3CollisionPoint, XMVectorAdd(origin, XMVectorScale(direction, length)));
                    }
                }
            }
            auto otherSphereCollider = dynamic_cast<SphereColliderComponent*>(other);
            if (otherSphereCollider)
            {
                otherSphereCollider->m_sphere.Intersects(origin, direction, length);
                if (0 < length)
                {
                    if (m_fMinLength > length)
                    {
                        m_fMinLength = length;
                        m_pCollided  = otherSphereCollider;
                        XMStoreFloat3(&m_xmf3CollisionPoint, XMVectorAdd(origin, XMVectorScale(direction, length)));
                    }
                }
            }
        }
        else
        {
            xmf3Origin    = muzzle->GetPosition(Space::world);
            xmf3Direction = muzzle->GetLookVector(Space::world);

            origin    = XMLoadFloat3(&xmf3Origin);
            direction = XMLoadFloat3(&xmf3Direction);

            auto otherBoxCollider = dynamic_cast<BoxColliderComponent*>(other);
            if (otherBoxCollider)
            {
                otherBoxCollider->m_box.Intersects(origin, direction, length);
                if (0 < length)
                {
                    if (m_fMinLength > length)
                    {
                        m_fMinLength = length;
                        m_pCollided  = otherBoxCollider;
                        XMStoreFloat3(&m_xmf3CollisionPoint, XMVectorAdd(origin, XMVectorScale(direction, length)));
                    }
                }
            }
            auto otherSphereCollider = dynamic_cast<SphereColliderComponent*>(other);
            if (otherSphereCollider)
            {
                otherSphereCollider->m_sphere.Intersects(origin, direction, length);
                if (0 < length)
                {
                    if (m_fMinLength > length)
                    {
                        m_fMinLength = length;
                        m_pCollided  = otherSphereCollider;
                        XMStoreFloat3(&m_xmf3CollisionPoint, XMVectorAdd(origin, XMVectorScale(direction, length)));
                    }
                }
            }
        }
    }
}

void WeaponControllerComponent::SolveConstraint()
{
    if (m_fTryRaycast)
    {
        m_fTryRaycast = false;
        m_fMinLength  = FLT_MAX;
        if (m_pCollided)
        {
            TempCharacter* enemy = m_pCollided->object->FindComponent<TempCharacter>();

            if (enemy) enemy->Damage(100);
        }
        m_pCollided = nullptr;

        {
            auto pe = new Object("particleEmitter");

            auto t   = new TransformComponent(pe);
            auto pec = new ParticleEmitterComponent(pe);

            pec->m_bIsBilboard      = false;
            pec->m_fGravityModifier = 0.0f;

            XMFLOAT3 muzzlePos = m_pMuzzle->FindComponent<TransformComponent>()->GetPosition(Space::world);

            pec->m_xmf3StartRotation = vector3::Normalize(vector3::Subtract(muzzlePos, m_xmf3CollisionPoint));

            t->Translate(m_xmf3CollisionPoint);
            //pec->SetMaterialByName("ParticleTestMat");
            pec->SetMaterialByName("ParticleSparkMat");
            pec->m_fStartSpeed     = fRange(15.0f, 20.0f);
            pec->m_nMaxParticles   = 100;
            pec->m_fDuration       = 2.0f;
            pec->m_fStartSize      = fRange(0.05f, 0.2f);
            pec->m_fRotationAngle  = 180;
            pec->m_fCreateCooltime = 5.0f;

            ParticleBurstInfo pb = {};
            pb.count             = 25;
            pb.isEnable          = true;
            pec->SetBurst(pb);

            g_pCurrScene->AddObject(pe, RenderGroup::PARTICLE);
        }
    }
}

void WeaponControllerComponent::Update(float fTimeElapsed)
{
    if (!is_enable) return;

    m_fCurrCooltime -= fTimeElapsed;

    if (m_bReloading)
    {
        m_fReloadProgress -= fTimeElapsed;
        if (0 >= m_fReloadProgress)
        {
            m_bReloading    = false;
            m_fCurrCooltime = 0;
            m_curAmmo       = m_maxAmmo;
        }
    }


    // Move to position of Parent's RHand
    if (object->m_pParent)
    {
        XMMATRIX l_xmmtxTransform = object->m_pParent->FindComponent<HumanoidAnimatorComponent>()->GetToWorldTransform(28);
        l_xmmtxTransform          = XMMatrixMultiply(XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(-90), XMConvertToRadians(-90)), l_xmmtxTransform);

        TransformComponent* transform = myTransform;
        transform->SetLocalTransform(l_xmmtxTransform);

        XMFLOAT3 xmf3Adjust(0, 0, 0);
        xmf3Adjust = vector3::Add(xmf3Adjust, vector3::Multiply(-0.028f, transform->GetRightVector()));
        xmf3Adjust = vector3::Add(xmf3Adjust, vector3::Multiply(0.08f, transform->GetLookVector()));
        transform->Translate(xmf3Adjust);
    }
}

void WeaponControllerComponent::Fire()
{
    if (!is_enable) return;
    if (m_bReloading) return;

    if (0 >= m_curAmmo)
    {
        Reload();
        return;
    }

    if (0 >= m_fCurrCooltime)
    {
        m_pMuzzle->FindComponent<EffectComponent>()->TurnOn();

        m_fCurrCooltime = m_fCooltime;
        m_fTryRaycast   = true;
        m_curAmmo--;
    }
}

void WeaponControllerComponent::Reload()
{
    m_bReloading      = true;
    m_fReloadProgress = m_fReloadTime;
}

void WeaponControllerComponent::SetCam(Object* pCam)
{
    camTransform = pCam->FindComponent<TransformComponent>();
    cam          = pCam->FindComponent<CameraComponent>();
}
