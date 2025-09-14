#include "pch.h"
#include "Presentation/Game/Component/Components.h"

TextUIAmmoComponent::TextUIAmmoComponent(Object* pObject, Object* pWeapon) : Component(pObject), m_pTRC(nullptr)
{
    TextRendererComponent* temp = pObject->FindComponent<TextRendererComponent>();
    if (temp) m_pTRC = temp;

    m_pWCC = pWeapon->FindComponent<WeaponControllerComponent>();
}

TextUIAmmoComponent::~TextUIAmmoComponent() {}

void TextUIAmmoComponent::Update(float fTimeElapsed)
{
    bool bReloading = m_pWCC->m_bReloading;

    string text;

    if (bReloading) text += "Reloading";
    else
    {
        text += "Ammo ";
        text += to_string(m_pWCC->m_curAmmo);
        text += "/";
        text += to_string(m_pWCC->m_maxAmmo);
    }

    m_pTRC->SetText(text.c_str());
}

TextUIScoreComponent::TextUIScoreComponent(Object* pObject) : Component(pObject), m_pTRC(nullptr)
{
    TextRendererComponent* temp = pObject->FindComponent<TextRendererComponent>();
    if (temp) m_pTRC = temp;
}

TextUIScoreComponent::~TextUIScoreComponent() {}

void TextUIScoreComponent::Update(float fTimeElapsed) {}

TextUIPlayerHPComponent::TextUIPlayerHPComponent(Object* pObject, Object* pPlayer) : Component(pObject), m_pTRC(nullptr), m_pPlayerCharacter(nullptr)
{
    TextRendererComponent* temp = pObject->FindComponent<TextRendererComponent>();
    if (temp) m_pTRC = temp;

    m_pPlayerCharacter = pPlayer->FindComponent<Character>();
}

TextUIPlayerHPComponent::~TextUIPlayerHPComponent() {}

void TextUIPlayerHPComponent::Update(float fTimeElapsed)
{
    string text = "HP ";

    text += to_string(m_pPlayerCharacter->GetCurrHp());

    m_pTRC->SetText(text.c_str());
}
