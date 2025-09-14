#include "pch.h"
#include "Presentation/Game/Component/Components.h"
#include "Presentation/Game/Object.h"
#include "Presentation/Game/Core/Scene.h"

TargetBoardControllerComponent::TargetBoardControllerComponent(Object* pObject, bool bAutoRevive) : Component(pObject), Character(10, bAutoRevive) {}

TargetBoardControllerComponent::~TargetBoardControllerComponent() {}

void TargetBoardControllerComponent::Update(float fTimeElapsed)
{
    if (!m_bEnabled) return;
    if (!this->isAlive()) return;

    float temp = floor(m_fTime);
    if (pe)
    {
        if (0 == temp) lpec->SetMaterialByName("p5");
        else if (1 == temp) lpec->SetMaterialByName("p4");
        else if (2 == temp) lpec->SetMaterialByName("p3");
        else if (3 == temp) lpec->SetMaterialByName("p2");
        else if (4 == temp) lpec->SetMaterialByName("p1");
    }

    if (m_fTime > m_fAttackPeriod)
    {
        m_pPlayerCharacter->Damage(10);
        Revive();
    }
    m_fTime += fTimeElapsed;

    Character::Update(fTimeElapsed);
}

void TargetBoardControllerComponent::Revive()
{
    Character::Revive();
    m_fTime = 0;

    {
        pe = new Object("particleEmitter");

        auto t   = new TransformComponent(pe);
        auto pec = new ParticleEmitterComponent(pe);

        t->Translate(m_pObject->FindComponent<TransformComponent>()->GetPosition(Space::world));
        t->Translate(0, 2.5, 0);
        pec->m_bIsBilboard      = true;
        pec->m_fGravityModifier = 0.0f;
        pec->SetMaterialByName("p5");
        pec->m_fStartSpeed     = fRange(0, 0);
        pec->m_nMaxParticles   = 6;
        pec->m_fDuration       = 5.0f;
        pec->m_fStartSize      = fRange(1, 1);
        pec->m_fCreateCooltime = 1.0f;
        pec->m_fStartLifetime  = fRange(1, 1);

        ParticleBurstInfo pb = {};
        pec->SetBurst(pb);

        g_pCurrScene->AddObject(pe, RenderGroup::PARTICLE);

        lpec = pec;
    }
}

void TargetBoardControllerComponent::Die()
{
    Character::Die();

    g_pCurrScene->event_count++;


    if (pe)
    {
        //pe->m_bEnable = false;	// �̰� ���� ��ƼŬ�� ��� ����
        lpec->m_bEnabled = false;

        pe   = nullptr;
        lpec = nullptr;
    }
}

void TargetBoardControllerComponent::SetPlayer(Object* pO) { m_pPlayerCharacter = pO->FindComponent<HumanoidControllerComponent>(); }
