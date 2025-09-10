#pragma once
#include "Component.h"

class TextUIAmmoComponent : public Component
{
public:
    TextUIAmmoComponent() = delete;
    TextUIAmmoComponent(Object* pObject, Object* pWeapon);
    ~TextUIAmmoComponent() override;

    void Update(float fTimeElapsed) override;

private:
    TextRendererComponent*     m_pTRC;
    WeaponControllerComponent* m_pWCC;
};

class TextUIScoreComponent : public Component
{
public:
    TextUIScoreComponent() = delete;
    TextUIScoreComponent(Object* pObject);
    ~TextUIScoreComponent() override;

    void Update(float fTimeElapsed) override;

private:
    TextRendererComponent* m_pTRC;
};

class TextUIPlayerHPComponent : public Component
{
public:
    TextUIPlayerHPComponent() = delete;
    TextUIPlayerHPComponent(Object* pObject, Object* pPlayer);
    ~TextUIPlayerHPComponent() override;

    void Update(float fTimeElapsed) override;

private:
    TextRendererComponent* m_pTRC;
    Character*             m_pPlayerCharacter;
};
