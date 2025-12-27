#include "pch.h"
#include "../../Engine/ECS/Components.h"
#include "../../Scene/Scene.h"

#include "src/Renderer/MaterialManager.h"

ParticleEmitterComponent::ParticleEmitterComponent(Object* pObject) : Component(pObject),
																	  m_fDuration(5.0f),
																	  m_bLooping(true),
																	  m_bPrewarm(false),
																	  m_fStartDelay(0),
																	  m_fStartLifetime(fRange(0.5, 1)),
																	  m_fStartSpeed(fRange(1, 1)),
																	  m_fStartSize(fRange(0.1f, 0.5f)),
																	  m_xmf3StartRotation(XMFLOAT3(0, 0, 1)),
																	  m_fRotationAngle(5),
																	  m_fFlipRotation(0),
																	  m_fGravityModifier(0.1f),
																	  m_RateOverTime(1),
																	  m_StopAction(ParticleStopAction::None),
																	  m_bIsBilboard(true),
																	  m_nMaxParticles(30),
																	  m_fCreateCooltime(1.0f / m_RateOverTime),
																	  m_fCurrCooltime(0),
																	  m_fTime(0.0f),
																	  m_pSceneParticlePool(&g_pCurrScene->m_vecParticlePool)
{
    ParticleBurstInfo pb = {};
    m_emissionBurst      = pb;
    m_fCurrBurstTime     = 0.0f;
}

ParticleEmitterComponent::~ParticleEmitterComponent() {}

void ParticleEmitterComponent::Update(float fTimeElapsed)
{
    // Update Particle
    // 占쏙옙占썩서 erase() 占싼다곤옙 占쌔쇽옙 占쏙옙占쏙옙 占쌍소곤옙 占쏙옙占쏙옙占쏙옙占신놂옙 占쏙옙占쏙옙 占쏙옙占쏙옙占실뤄옙 占싫쏙옙占싹곤옙 erase() 占쌔듸옙 占싫댐옙.
    for (auto iter = m_vecParticle.begin(); iter != m_vecParticle.end();)
    {
        (*iter)->Update(fTimeElapsed);
        if (!(*iter)->m_bEnable) iter = m_vecParticle.erase(iter);
        else ++iter;
    }

    if (!m_bEnabled) return;

    // Update Emitter

    // Stop Particle Emitter
    if (m_fTime > m_fDuration)
    {
        if (m_fTime > m_fDuration + m_fStartLifetime._max)
        {
            m_pObject->m_bEnable = false;
            m_bEnabled           = false;
            m_vecParticle.clear();
        }
    }
    else
    {
        // for Test!
        if (m_fCreateCooltime < m_fCurrCooltime && m_nMaxParticles > m_vecParticle.size())
        {
            m_fCurrCooltime = 0;
            AddParticle();
        }
        m_fCurrCooltime += fTimeElapsed;

        if (m_emissionBurst.isEnable && m_emissionBurst.time < m_fCurrBurstTime)
        {
            for (int i = 0; i < m_emissionBurst.count; i++) if (m_nMaxParticles > m_vecParticle.size()) AddParticle();
            m_fCurrBurstTime         = 0;
            m_emissionBurst.isEnable = false;
        }
        m_fCurrBurstTime += fTimeElapsed;
    }

    //AddParticle();


    m_fTime += fTimeElapsed;
}

void ParticleEmitterComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
    // m_strMaterialName占쏙옙 ""占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙~ 占쏙옙占쏙옙占쏙옙.
    // meshRenderer占쏙옙 占쌕몌옙占쏙옙 particle占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙 render()占쏙옙 占쏙옙占쌩곤옙 占쌍억옙占?占싼댐옙.
    // Particle占쏙옙 material占쏙옙 占쏙옙占쏙옙 占십는댐옙. material Set占쏙옙 占쏙옙占썩서 占쏙옙 占쏙옙 占싹곤옙 占쏙옙 占쌘뤄옙 render()占쏙옙 占쏙옙.
    g_MaterialMng.SetMaterial(m_strMaterialName.c_str(), pd3dCommandList);
    for_each(m_vecParticle.begin(), m_vecParticle.end(), [&](Object* o) { o->Render(pd3dCommandList); });
}

void ParticleEmitterComponent::SetMaterialByName(const char* strMaterialName) { m_strMaterialName = strMaterialName; }

void ParticleEmitterComponent::SetBurst(ParticleBurstInfo& burstInfo) { m_emissionBurst = burstInfo; }

void ParticleEmitterComponent::AddParticle()
{
    // Access SceneParticlePool, Get disabled Particle Object, Init them, add to m_vecParticle;
    Object* particle = GetUsableParticle();
    if (particle)
    {
        InitializeParticle(particle);
        m_vecParticle.push_back(particle);
    }
    else
    {
        // 占쏙옙占쏙옙 占쏙옙 占쏙옙~
    }
}

Object* ParticleEmitterComponent::GetUsableParticle()
{
    for (int i = 0; i < m_pSceneParticlePool->size(); i++) if (!(*m_pSceneParticlePool)[i]->m_bEnable) return (*m_pSceneParticlePool)[i];
    return nullptr;
}

float GetRandomFloat(float min, float max)
{
    // min 5, max 10
    // rand() % (max-min) -> 0 ~ 5
    // rand() % (max-min) + min -> 5 ~ 10
    // rand() % ((max-min) * 1000) -> 0 ~ 5000
    // rand() % ((max-min) * 1000) * 0.001 -> 0 ~ 5.000
    // rand() % ((max-min) * 1000) * 0.001 + min -> 5 ~ 10.000
    if (min == max) return min;
    return rand() % static_cast<int>((max - min) * 1000) * 0.001 + min;
}

XMFLOAT3 GetRandomlyShakenVector(const XMFLOAT3& d, float angle)
{
    XMFLOAT3 result;
    float    deg = XMConvertToRadians(angle);

    XMStoreFloat3(&result, XMVector3Rotate(XMLoadFloat3(&d), XMQuaternionRotationRollPitchYaw(GetRandomFloat(0, deg) - deg * 0.5f, GetRandomFloat(0, deg) - deg * 0.5f, GetRandomFloat(0, deg) - deg * 0.5f)));

    return result;
}

void ParticleEmitterComponent::InitializeParticle(Object* pObject)
{
    ParticleComponent*     pc = pObject->FindComponent<ParticleComponent>();
    TransformComponent*    t  = m_pObject->FindComponent<TransformComponent>();
    ParticlePropertiesPack ppp;

    ppp.currPos    = t->GetPosition();
    ppp.direction  = GetRandomlyShakenVector(m_xmf3StartRotation, m_fRotationAngle);
    ppp.gravity    = m_fGravityModifier;
    ppp.lifetime   = GetRandomFloat(m_fStartLifetime._min, m_fStartLifetime._max);
    ppp.size       = GetRandomFloat(m_fStartSize._min, m_fStartSize._max);
    ppp.speed      = GetRandomFloat(m_fStartSpeed._min, m_fStartSpeed._max);
    ppp.isBilboard = m_bIsBilboard;

    pc->Initialize(ppp);
}
