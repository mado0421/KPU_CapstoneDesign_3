#pragma once
#include "../../Engine/ECS/Component.h"

struct fRange
{
    float _min, _max;

    fRange(float min, float max) : _min(min), _max(max) {}
};

enum class ParticleStopAction
{
    None,
    Disable,
    Destroy
};

struct ParticleBurstInfo
{
    float time        = 0.0f;
    int   count       = 0;
    int   cycles      = 0; //0:infinit
    float interval    = 5.0f;
    float probability = 1.0f; //0~1
    bool  isEnable    = false;
};

struct ParticlePropertiesPack
{
    float    lifetime;
    XMFLOAT3 currPos;
    XMFLOAT3 direction;
    float    speed;
    float    size;
    float    gravity;
    bool     isBilboard;
};

class ParticleEmitterComponent : public Component
{
public:
    ParticleEmitterComponent() = delete;
    ParticleEmitterComponent(Object* pObject);
    ~ParticleEmitterComponent() override;

    void Update(float fTimeElapsed) override;
    void Render(ID3D12GraphicsCommandList* pd3dCommandList) override;

    void SetMaterialByName(const char* strMaterialName);
    void SetBurst(ParticleBurstInfo& burstInfo);

private:
    void AddParticle();

    Object* GetUsableParticle();
    void    InitializeParticle(Object* pObject);

public:
    float              m_fDuration;
    bool               m_bLooping;
    bool               m_bPrewarm;
    float              m_fStartDelay;
    fRange             m_fStartLifetime;
    fRange             m_fStartSpeed;
    fRange             m_fStartSize;
    XMFLOAT3           m_xmf3StartRotation;
    float              m_fRotationAngle;
    float              m_fFlipRotation;
    float              m_fGravityModifier;
    int                m_RateOverTime;
    ParticleStopAction m_StopAction;
    ParticleBurstInfo  m_emissionBurst;
    bool               m_bIsBilboard;
    int                m_nMaxParticles;

    float m_fCreateCooltime;
    float m_fCurrCooltime;
    float m_fCurrBurstTime;
    float m_fTime;

protected:
    vector<Object*> m_vecParticle;
    string          m_strMaterialName;

    vector<Object*>* m_pSceneParticlePool;
};
