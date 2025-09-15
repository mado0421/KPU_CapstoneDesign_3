#pragma once
#include "Component.h"

class TextUIAmmoComponent : public Component
{
public:
    TextUIAmmoComponent() = delete;
    TextUIAmmoComponent(GameObject* pObject, GameObject* pWeapon);
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
    TextUIScoreComponent(GameObject* pObject);
    ~TextUIScoreComponent() override;

    void Update(float fTimeElapsed) override;

private:
    TextRendererComponent* m_pTRC;
};

class TextUIPlayerHPComponent : public Component
{
public:
    TextUIPlayerHPComponent() = delete;
    TextUIPlayerHPComponent(GameObject* pObject, GameObject* pPlayer);
    ~TextUIPlayerHPComponent() override;

    void Update(float fTimeElapsed) override;

private:
    TextRendererComponent* m_pTRC;
    TempCharacter*             m_pPlayerCharacter;
};
