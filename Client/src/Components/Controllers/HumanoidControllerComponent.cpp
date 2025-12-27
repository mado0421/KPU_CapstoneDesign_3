#include "pch.h"
#include "HumanoidControllerComponent.h"
#include "../../Engine/ECS/Components.h"
#include "../../Engine/ECS/Object.h"
#include "../../Scene/Scene.h"

HumanoidControllerComponent::HumanoidControllerComponent(Object* pObject, Object* pWeapon) : Component(pObject),
                                                                                             Character(100, true, false),
                                                                                             m_fTime(0),
                                                                                             m_xmf3Velocity(0, 0, 0),
                                                                                             m_fSpeed(1.0f),
                                                                                             m_fDragFactor(1.0f),
                                                                                             m_fAimProgress(0),
                                                                                             m_fTimeForAim(0.2f),
                                                                                             m_pWeaponObject(pWeapon)
{
    l_pInput    = m_pObject->FindComponent<InputManagerComponent>();
    l_transform = m_pObject->FindComponent<TransformComponent>();
}

HumanoidControllerComponent::~HumanoidControllerComponent() {}

void HumanoidControllerComponent::SetLookAt(Object* pObejct) { m_pLookAt = pObejct->FindComponent<TransformComponent>(); }

void HumanoidControllerComponent::Damage(int dmg)
{
    Character::Damage(dmg);

    {
        auto pe = new Object("particleEmitter");

        auto t   = new TransformComponent(pe);
        auto pec = new ParticleEmitterComponent(pe);

        t->Translate(m_pObject->FindComponent<TransformComponent>()->GetPosition(Space::world));
        t->Translate(0, 1.5, 0);
        pec->m_bIsBilboard      = false;
        pec->m_fGravityModifier = 0.3f;
        pec->SetMaterialByName("ParticleDam");
        pec->m_fStartSpeed     = fRange(0.1, 1.0);
        pec->m_nMaxParticles   = 1;
        pec->m_fDuration       = 0.2f;
        pec->m_fStartSize      = fRange(0.3, 0.7);
        pec->m_fCreateCooltime = 0.0f;
        pec->m_fStartLifetime  = fRange(0.1, 0.2);

        ParticleBurstInfo pb = {};
        pec->SetBurst(pb);

        g_pCurrScene->AddObject(pe, RenderGroup::PARTICLE);
    }
}

void HumanoidControllerComponent::Update(float fTimeElapsed)
{
    if (!m_bEnabled) return;

    m_fTime += fTimeElapsed;

    if (l_pInput->IsKeyDown(_P)) Character::Damage(100);


    if (!this->isAlive()) g_pCurrScene->Clear();
    else
    {
        // Movement Part
        {
            XMFLOAT2 xmf2MouseMovement = l_pInput->GetMouseMovement();

            // for test
            if (0 != xmf2MouseMovement.x) l_transform->RotateXYZDegree(0, xmf2MouseMovement.x * -0.05f, 0);

            // for test
            if (m_pLookAt && 0 != xmf2MouseMovement.y)
            {
                float fTemp = lookAtYAngle + xmf2MouseMovement.y * -0.001f;
                if (-1.5 < fTemp && fTemp < 1.5)
                {
                    lookAtYAngle = fTemp;

                    m_pLookAt->SetPosition(0, 1.5f + lookAtYAngle, 5.0f);
                }
            }
        }
        {
            auto l_xmf3Direction = XMFLOAT3(0, 0, 0);

            if (l_pInput->IsKeyDown(_W)) l_xmf3Direction.z += m_fSpeed;
            if (l_pInput->IsKeyDown(_A)) l_xmf3Direction.x -= m_fSpeed;
            if (l_pInput->IsKeyDown(_S)) l_xmf3Direction.z -= m_fSpeed;
            if (l_pInput->IsKeyDown(_D)) l_xmf3Direction.x += m_fSpeed;

            float fPrevSpd  = Vector3::Length(m_xmf3Velocity);
            float fSpdLimit = 1.5f;
            if (!Vector3::IsZero(l_xmf3Direction))
            {
                m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multiply(fTimeElapsed * 2.8, Vector3::Normalize(l_xmf3Direction)));
                Clamp(m_xmf3Velocity.x, -fSpdLimit, fSpdLimit);
                Clamp(m_xmf3Velocity.z, -fSpdLimit, fSpdLimit);
            }

            if (0 == l_xmf3Direction.x)
            {
                if (m_xmf3Velocity.x > 0) m_xmf3Velocity.x -= m_fDragFactor * fTimeElapsed;
                else m_xmf3Velocity.x += m_fDragFactor * fTimeElapsed;
                if (m_fDragFactor * fTimeElapsed >= fabs(m_xmf3Velocity.x)) m_xmf3Velocity.x = 0;
            }
            if (0 == l_xmf3Direction.z)
            {
                if (m_xmf3Velocity.z > 0) m_xmf3Velocity.z -= m_fDragFactor * fTimeElapsed;
                else m_xmf3Velocity.z += m_fDragFactor * fTimeElapsed;
                if (m_fDragFactor * fTimeElapsed >= fabs(m_xmf3Velocity.z)) m_xmf3Velocity.z = 0;
            }

            XMFLOAT3 xmf3Velocity;
            XMFLOAT4 xmf4RotationQuaternion;
            XMVECTOR velocity           = XMLoadFloat3(&m_xmf3Velocity);
            xmf4RotationQuaternion      = l_transform->GetRotationQuaternion(Space::world);
            XMVECTOR rotationQuaternion = XMLoadFloat4(&xmf4RotationQuaternion);
            velocity                    = XMVector3Rotate(velocity, rotationQuaternion);
            XMStoreFloat3(&xmf3Velocity, velocity);

            l_transform->Translate(Vector3::Multiply(fTimeElapsed, xmf3Velocity));


            //// for Test
            //xmf3Velocity = l_transform->GetPosition(Space::world);
            //cout << xmf3Velocity.x << ", " << xmf3Velocity.y << ", " << xmf3Velocity.z << "\n";
        }

        // Action Part
        {
            if (l_pInput->IsKeyDown(_LMB))
            {
                if (m_fTimeForAim > m_fAimProgress) m_fAimProgress += fTimeElapsed;
                else m_pWeaponObject->FindComponent<WeaponControllerComponent>()->Fire();
            }
            else
            {
                m_fAimProgress -= fTimeElapsed;
                if (0 > m_fAimProgress) m_fAimProgress = 0;
            }

            if (l_pInput->IsKeyDown(_R)) m_pWeaponObject->FindComponent<WeaponControllerComponent>()->Reload();
        }
    }

    Character::Update(fTimeElapsed);
}
